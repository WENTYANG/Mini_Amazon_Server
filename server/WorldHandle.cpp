#include "WorldHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>
#include <mutex>

#include "./protobuf/AUprotocolV3.pb.h"
#include "./protobuf/world_amazon.pb.h"
#include "exception.h"
#include "server.h"
#include "sql_functions.h"
#include "working_threads.h"

using namespace std;

// use a global unordered_set to keep track of all seq_number we received
// Guarantee an idiompotent behaviour
unordered_set<int64_t> seq_nums;

// send an ack for every message with an seqnum
bool sendAck(AResponses& response, proto_out* world_out, mutex& mtx) {
  ACommands cmd;
  for (int i = 0; i < response.arrived_size(); ++i) {
    int64_t seq = response.arrived(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.ready_size(); ++i) {
    int64_t seq = response.ready(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.loaded_size(); ++i) {
    int64_t seq = response.loaded(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.error_size(); ++i) {
    int64_t seq = response.error(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.packagestatus_size(); ++i) {
    int64_t seq = response.packagestatus(i).seqnum();
    cmd.add_acks(seq);
  }
  mtx.lock();
  if (sendMesgTo<ACommands>(cmd, world_out) == false) {
    mtx.unlock();
    throw MyException("Error occured when sending ACommands World");
  }
  mtx.unlock();
  return true;
}

void RecvFromWorld(proto_in* world_in) {
    while (1) {
        try {
            AResponses response;
            if (recvMesgFrom<AResponses>(response, world_in) == false) {
                throw MyException(
                    "Error occured when receiving AResponse from World");
            }
            // Parse AResponses and handle
            cout << "Received from world: " << response.DebugString() << std::endl;
            Server& s = Server::get_instance();
            sendAck(response, s.world_out, s.world_out_mtx);
            for (int i = 0; i < response.arrived_size(); ++i) {
              // update database, add more inventory
              const APurchaseMore& apm = response.arrived(i);
              int64_t seq = apm.seqnum();
              if (seq_nums.find(seq) != seq_nums.end()) {
                continue;
              } else {
                seq_nums.insert(seq);
              }
              int whnum = apm.whnum();
              for (int j = 0; j < apm.things_size(); ++j) {
                const AProduct& product = apm.things(j);
                add_inventory(whnum, (int)product.id(), product.count());
              }
            }
            for (int i = 0; i < response.ready_size(); ++i) {
              // updata database and check if ups truck also arrived, if arrived, spawn load thread
              const APacked& aped = response.ready(i);
              int64_t seq = aped.seqnum();
              if (seq_nums.find(seq) != seq_nums.end()) {
                continue;
              } else {
                seq_nums.insert(seq);
              }
              bool truck_arrived = false;
              try {
                truck_arrived = packed_and_check_ups_truck(aped.shipid());
              } catch (MyException& e) {
                cout << e.what() << endl;
                continue;
              }
              if (truck_arrived) {
                int i_id = aped.shipid();
                s.threadPool->assign_task(bind(load, i_id));
              }
            }
            for (int i = 0; i < response.loaded_size(); ++i) {
              // update database and spwan ready to deliver thread
              const ALoaded& aled = response.loaded(i);
              int64_t seq = aled.seqnum();
              if (seq_nums.find(seq) != seq_nums.end()) {
                continue;
              } else {
                seq_nums.insert(seq);
              }
              try {
                change_status_to_delivering(aled.shipid());
              } catch (MyException& e) {
                cout << e.what() << endl;
                continue;
              }
              int i_id = aled.shipid();
              s.threadPool->assign_task(bind(ready_to_deliver, i_id));
            }
            if (response.has_finished()){
              //TODO: exit?
              cout << "World has finished processing all commands and is ready to quit.\n";
            }
            for (int i = 0; i < response.error_size(); ++i) {
              // TODO: Just print out error info for now
              const AErr& aerr = response.error(i);
              cerr << "Got an error message for seq=" << aerr.originseqnum() << " msg: " << aerr.err() << endl;
            }
            for (int i = 0; i < response.acks_size(); ++i) {
              int64_t ack = response.acks(i);
              // TODO: update ack,Timer map
            }
            for (int i = 0; i < response.packagestatus_size(); ++i) {
              // TODO:Currently just printing out info
              // Our server should never sent Aquery and never receive APackage
              const APackage& ps = response.packagestatus(i);
              cout << "Get package status for package id=" << ps.packageid() << " status=" << ps.status() << endl;
            }

        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

#include "WorldHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>

#include "./protobuf/AUprotocolV3.pb.h"
#include "./protobuf/world_amazon.pb.h"
#include "exception.h"
#include "proto.h"
#include "server.h"
#include "sql_function.h"

using namespace std;

// send an ack for every message with an syn
bool sendAck(AResponses& response) {
}


void RecvFromWorld(proto_in* world_in) {
    while (1) {
        try {
            AResponses response;
            if (recvMesgFrom<AResponses>(response, world_in) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            // TO DO: Parse AResponses and handle
            cout << "Received from world: " << response.DebugString() << std::endl;
            sendAck(response);
            for (int i = 0; i < response.arrived_size(); ++i) {
              // update database, add more inventory
              APurchaseMore& apm = response.arrived(i);
              int32 whnum = apm.whnum;
              for (int j = 0; j < apm.things_size(); ++j) {
                AProduct& product = apm.things(j);
                add_inventory(whnum, (int)product.id, product.count);
              }
            }
            for (int i = 0; i < response.ready_size(); ++i) {
              // updata database and check if ups truck also arrived, if arrived, spawn load thread
              APacked& aped = response.ready(i);
              try {
                bool truck_arrived = packed_and_check_ups_truck(aped.shipid);
              } catch (MyException& e) {
                cout << e.what() << endl;
                continue;
              }
              if (truck_arrived) {
                Server& s = Server::get_instance();
                s.threadPool->assign_task(bind(load, aped.shipid));
              }
            }
            for (int i = 0; i < response.loaded_size(); ++i) {
              // update database and spwan ready to deliver thread
              ALoaded& aled = response.loaded(i);
              try {
                change_status_to_delivering(aled.shipid);
              } catch (MyException& e) {
                cout << e.what() << endl;
                continue;
              }
              Server& s = Server::get_instance();
              s.threadPool->assign_task(bind(ready_to_deliver, aled.shipid));
            }
            if (response.has_finished()){
              //TODO
            }
            for (int i = 0; i < response.error_size(); ++i) {
              // Just print out error info for now
              AErr& aerr = response.error(i);
              cerr << "Got an error message for seq=" << aerr.originseqnum << " msg: " << aerr.err << endl;
            }
            for (int i = 0; i < response.acks_size(); ++i) {
              int64 ack = response.acks(i);
              // TODO: update ack,Timer map
            }
            for (int i = 0; i < response.packagestatus_size(); ++i) {
              // Currently just printing out info
              APackage& ps = response.packagestatus(i);
              cout << "Get package status for package id=" << ps.packageid << " status=" << ps.status << endl;
            }

        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}
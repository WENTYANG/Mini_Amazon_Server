#include "WorldHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <mutex>
#include <string>
#include <vector>

#include "./protobuf/AUprotocolV3.pb.h"
#include "./protobuf/world_amazon.pb.h"
#include "ThreadSafe_queue.h"
#include "exception.h"
#include "server.h"
#include "sql_functions.h"
#include "working_threads.h"

using namespace std;
#define SEND_BATCH 3

// use a global unordered_set to keep track of all seq_number we received
// Guarantee an idiompotent behaviour
static unordered_set<int64_t> seq_nums;

void RecvFromWorld(proto_in* world_in) {
    while (1) {
        try {
            AResponses response;
            if (recvMesgFrom<AResponses>(response, world_in) == false) {
                throw MyException(
                    "Error occured when receiving AResponse from World");
            }
            // Parse AResponses and handle
            cout << "Received from world: " << response.DebugString()
                 << std::endl;
            Server& s = Server::get_instance();
            s.threadPool->assign_task(bind(sendAck_world, response));
            for (int i = 0; i < response.arrived_size(); ++i) {
                // update database, add more inventory
                const APurchaseMore& apm = response.arrived(i);
                int64_t seq = apm.seqnum();
                if (seq_nums.find(seq) != seq_nums.end()) {
                    continue;
                } else {
                    seq_nums.insert(seq);
                }
                s.threadPool->assign_task(bind(purchase_more, apm));
            }
            for (int i = 0; i < response.ready_size(); ++i) {
                // updata database and check if ups truck also arrived, if
                // arrived, spawn load thread
                const APacked& aped = response.ready(i);
                int64_t seq = aped.seqnum();
                if (seq_nums.find(seq) != seq_nums.end()) {
                    continue;
                } else {
                    seq_nums.insert(seq);
                }
                s.threadPool->assign_task(bind(load_world, aped));
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
                s.threadPool->assign_task(bind(ready_to_deliver, aled));
            }
            if (response.has_finished()) {
                // TODO: exit?
                cout << "World has finished processing all commands and is "
                        "ready to quit.\n";
            }
            for (int i = 0; i < response.error_size(); ++i) {
                // TODO: Just print out error info for now
                const AErr& aerr = response.error(i);
                cerr << "Got an error message for seq=" << aerr.originseqnum()
                     << " msg: " << aerr.err() << endl;
            }
            for (int i = 0; i < response.acks_size(); ++i) {
                int64_t ack = response.acks(i);
                // TODO: update ack, Timer map
            }
            for (int i = 0; i < response.packagestatus_size(); ++i) {
                // TODO:Currently just printing out info
                // Our server should never sent Aquery and never receive
                // APackage
                const APackage& ps = response.packagestatus(i);
                cout << "Get package status for package id=" << ps.packageid()
                     << " status=" << ps.status() << endl;
            }

        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

/*
  Dedicated output thread to pop requests out of world queue and send to world
  Send at most SEND_BATCH requests in 1 ACommands at a time
*/
void SendToWorld(proto_out* world_out) {
    Server& s = Server::get_instance();
    ThreadSafe_queue<ACommands>& que = s.world_output_queue;
    while (1) {
        try {
            ACommands cToSend;
            for (int j = 0; j < SEND_BATCH; j++) {
                if (que.empty()) {
                    break;
                }
                // pop
                ACommands request;
                que.try_pop(request);
                // parse & add seqnum & append to commandToSend
                for (int i = 0; i < request.buy_size(); ++i) {
                    const auto& currBuy = request.buy(i);
                    auto buy = cToSend.add_buy();
                    buy->CopyFrom(currBuy);
                    buy->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.topack_size(); ++i) {
                    const auto& currPack = request.topack(i);
                    auto topack = cToSend.add_topack();
                    topack->CopyFrom(currPack);
                    topack->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.load_size(); ++i) {
                    const auto& currLoad = request.load(i);
                    auto load = cToSend.add_load();
                    load->CopyFrom(currLoad);
                    load->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.queries_size(); ++i) {
                    const auto& currQuery = request.queries(i);
                    auto queries = cToSend.add_queries();
                    queries->CopyFrom(currQuery);
                    queries->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.acks_size(); ++i) {
                    cToSend.add_acks(request.acks(i));
                }
                if (request.has_disconnect()) {
                    cToSend.set_disconnect(request.disconnect());
                }
                if (request.has_simspeed()) {
                    cToSend.set_simspeed(request.simspeed());
                }
            }
            if (cToSend.buy_size() > 0 || cToSend.topack_size() > 0 ||
                cToSend.load_size() > 0 || cToSend.queries_size() > 0 ||
                cToSend.acks_size() > 0 || cToSend.has_disconnect() ||
                cToSend.has_simspeed()) {
                // send
                if (sendMesgTo<ACommands>(cToSend, world_out) == false) {
                    throw MyException(
                        "sendToWorld thread: Error occured when sending "
                        "ACommands "
                        "to World.");
                }

                cout << "Send to world: " << cToSend.DebugString()
                    << std::endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

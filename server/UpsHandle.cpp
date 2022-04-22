#include "UpsHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>

#include "./protobuf/AUprotocolV4.pb.h"
#include "exception.h"
#include "server.h"
#include "sql_functions.h"
#include "working_threads.h"

using namespace std;
#define SEND_BATCH 3

static unordered_set<int64_t> seq_nums;

void RecvFromUps(proto_in* ups_in) {
    while (1) {
        try {
            UACommand response;
            if (recvMesgFrom<UACommand>(response, ups_in) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            cout << "Received from UPS: " << response.DebugString()
                 << std::endl;
            Server& s = Server::get_instance();
            s.threadPool->assign_task(bind(sendAck_ups, response));
            for (int i = 0; i < response.arrive_size(); ++i) {
                const UTruckArrive& uta = response.arrive(i);
                int64_t seq = uta.seqnum();
                if (seq_nums.find(seq) != seq_nums.end()) {
                    continue;
                } else {
                    seq_nums.insert(seq);
                }
                s.threadPool->assign_task(bind(load_ups, uta));
            }
            for (int i = 0; i < response.delivered_size(); ++i) {
                const UDelivered& uded = response.delivered(i);
                int64_t seq = uded.seqnum();
                if (seq_nums.find(seq) != seq_nums.end()) {
                    continue;
                } else {
                    seq_nums.insert(seq);
                }
                s.threadPool->assign_task(bind(delivered, uded));
            }
            for (int i = 0; i < response.acks_size(); ++i) {
                int64_t ack = response.acks(i);
                // TODO: update ack, Timer map
                s.UPS_sent.erase(ack);
            }
            for (int i = 0; i < response.error_size(); ++i) {
                const Err& err = response.error(i);
                cerr << "Got an error message for seq=" << err.originseqnum()
                     << " msg: " << err.err() << endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

/*
  Dedicated output thread to pop requests out of ups queue and send to ups
  Send at most SEND_BATCH requests in 1 AUCommand at a time
*/
void SendToUps(proto_out* ups_out) {
    Server& s = Server::get_instance();
    ThreadSafe_queue<AUCommand>& que = s.ups_output_queue;
    while (1) {
        try {
            AUCommand cToSend;
            for (int j = 0; j < SEND_BATCH; j++) {
                if (que.empty()) {
                    break;
                }
                // pop
                AUCommand request;
                que.try_pop(request);
                // parse & add seqnum & append to commandToSend
                for (int i = 0; i < request.deliver_size(); i++) {
                    const auto& currDeliver = request.deliver(i);
                    auto deliver = cToSend.add_deliver();
                    deliver->CopyFrom(currDeliver);
                    if (currDeliver.seqnum() == 0) {
                      deliver->set_seqnum(s.getSeqNum());
                      int64_t seq = deliver->seqnum();
                      AUCommand cmd;
                      auto cmd_deliver = cmd.add_deliver();
                      cmd_deliver->CopyFrom(*deliver);
                      unique_ptr<TimerResendUps> ptr(new TimerResendUps(cmd));
                      ptr->start();
                      s.UPS_sent[seq] = move(ptr);
                    }
                }
                for (int i = 0; i < request.order_size(); i++) {
                    const auto& currOrder = request.order(i);
                    auto order = cToSend.add_order();
                    order->CopyFrom(currOrder);
                    if (currOrder.seqnum() == 0) {
                      order->set_seqnum(s.getSeqNum());
                      int64_t seq = order->seqnum();
                      AUCommand cmd;
                      auto cmd_order = cmd.add_order();
                      cmd_order->CopyFrom(*order);
                      unique_ptr<TimerResendUps> ptr(new TimerResendUps(cmd));
                      ptr->start();
                      s.UPS_sent[seq] = move(ptr);
                    }
                }
                for (int i = 0; i < request.acks_size(); i++) {
                    cToSend.add_acks(request.acks(i));
                }
                for (int i = 0; i < request.error_size(); i++) {
                    const Err& currErr = request.error(i);
                    auto err = cToSend.add_error();
                    err->CopyFrom(currErr);
                    if (currErr.seqnum() == 0) {
                      err->set_seqnum(s.getSeqNum());
                      int64_t seq = err->seqnum();
                      AUCommand cmd;
                      auto cmd_err = cmd.add_error();
                      cmd_err->CopyFrom(*err);
                      unique_ptr<TimerResendUps> ptr(new TimerResendUps(cmd));
                      ptr->start();
                      s.UPS_sent[seq] = move(ptr);
                    }
                }
            }
            if (cToSend.deliver_size() > 0 || cToSend.order_size() > 0 ||
                cToSend.acks_size() > 0 || cToSend.error_size() > 0) {
                if (sendMesgTo<AUCommand>(cToSend, ups_out) == false) {
                    throw MyException(
                        "sendToUps  thread: Error occured when sending "
                        "AUCommand "
                        "to UPS.");
                }
                cout << "Send to ups: " << cToSend.DebugString() << std::endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

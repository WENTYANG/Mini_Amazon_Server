#include "UpsHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>

#include "./protobuf/world_amazon.pb.h"
#include "exception.h"
#include "server.h"

using namespace std;
#define SEND_BATCH 3

void RecvFromUps(proto_in* ups_in) {
    while (1) {
        try {
            AUResponse response;
            if (recvMesgFrom<AUResponse>(response, ups_in) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            // Parse AUResponse
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

/*
  Dedicated output thread to pop requests out of ups queue and send to ups
  Send at most SEND_BATCH requests in 1 AUCommand at a time
*/
void sendToUps() {
    Server& s = Server::get_instance();
    ThreadSafe_queue<AUCommand>& que = s.ups_output_queue;
    proto_out* ups_out = s.ups_out;
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
                    deliver->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.order_size(); i++) {
                    const auto& currOrder = request.order(i);
                    auto order = cToSend.add_order();
                    order->CopyFrom(currOrder);
                    order->set_seqnum(s.getSeqNum());
                }
                for (int i = 0; i < request.acks_size(); i++) {
                    cToSend.add_acks(request.acks(i));
                }
                for (int i = 0; i < request.error_size(); i++) {
                    const Err& currErr = request.error(i);
                    auto err = cToSend.add_error();
                    err->CopyFrom(currErr);
                    err->set_seqnum(s.getSeqNum());
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
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

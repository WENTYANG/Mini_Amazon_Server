#include "UpsHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>

#include "./protobuf/AUprotocolV3.pb.h"
#include "exception.h"
#include "server.h"
#include "sql_functions.h"
#include "working_threads.h"

using namespace std;

static unordered_set<int64_t> seq_nums;

void RecvFromUps(proto_in * ups_in) {
    while (1) {
        try {
            UACommand response;
            if (recvMesgFrom<UACommand>(response, ups_in) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            cout << "Received from UPS: " << response.DebugString() << std::endl;
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
            }
            for (int i = 0; i < response.error_size(); ++i) {
              const Err& err = response.error(i);
              cerr << "Got an error message for seq=" << err.originseqnum() << " msg: " << err.err() << endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}

#include "UpsHandle.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <vector>

#include "./protobuf/AUprotocolV3.pb.h"
#include "./protobuf/world_amazon.pb.h"
#include "exception.h"
#include "proto.h"
#include "server.h"

using namespace std;

void RecvFromUps() {
    unique_ptr<proto_in> ups_in(new proto_in(Server::get_instance().ups_fd));
    while (1) {
        try {
            AUResponse response;
            if (recvMesgFrom<AUResponse>(response, ups_in.get()) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            // Parse AUResponse
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}
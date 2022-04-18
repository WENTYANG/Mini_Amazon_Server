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

using namespace std;

void RecvFromWorld() {
    unique_ptr<proto_in> world_in(
        new proto_in(Server::get_instance().world_fd));
    while (1) {
        try {
            AResponses response;
            if (recvMesgFrom<AResponses>(response, world_in.get()) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            // TO DO: Parse AResponses and handle
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}
#ifndef _WORLDHANDLE_H
#define _WORLDHANDLE_H

#include "./protobuf/world_amazon.pb.h"
#include "proto.h"

bool sendAck(AResponses& response);
void RecvFromWorld(proto_in* world_in);
void sendToWorld();

#endif

#ifndef _UPSHANDLE_H
#define _UPSHANDLE_H

#include "./protobuf/AUprotocolV3.pb.h"
#include "proto.h"

void RecvFromUps(proto_in* ups_in);
void SendToUps(proto_out* ups_out);

#endif

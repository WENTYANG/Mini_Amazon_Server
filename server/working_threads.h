#ifndef _WORKING_THREADS_H
#define _WORKING_THREADS_H
#include "sql_functions.h"
#include "./protobuf/world_amazon.pb.h"

void sendAck(AResponses response);
void purchase_more(APurchaseMore apm);
void load(APacked aped);
void ready_to_deliver(ALoaded aled);

#endif

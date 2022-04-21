#ifndef _WORKING_THREADS_H
#define _WORKING_THREADS_H
#include "./protobuf/world_amazon.pb.h"
#include "sql_functions.h"
#include "warehouse.h"

void sendAck(AResponses response);
void purchase_more(APurchaseMore apm);
void load(APacked aped);
void ready_to_deliver(ALoaded aled);
void pack(shared_ptr<SubOrder> order, int w_id);

#endif

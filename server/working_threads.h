#ifndef _WORKING_THREADS_H
#define _WORKING_THREADS_H
#include "sql_functions.h"
#include "./protobuf/world_amazon.pb.h"
#include "./protobuf/AUprotocolV3.pb.h"

// for communication with world
void sendAck_world(AResponses response);
void purchase_more(APurchaseMore apm);
void load_world(APacked aped);
void ready_to_deliver(ALoaded aled);

// for communication with UPS
void sendAck_ups(UACommand response);
void load_ups(UTruckArrive uta);
void delivered(UDelivered uded);
#endif

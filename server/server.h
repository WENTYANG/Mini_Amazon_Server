#ifndef _SERVER_H
#define _SERVER_H

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "./protobuf/world_amazon.pb.h"
#include "exception.h"
#include "threadpool.h"

using namespace std;
using namespace pqxx;
/*
int32_t id() const;
int32_t x() const;
int32_t y() const;
*/

class Warehouse {
   public:
    int id;
    int x;
    int y;
    Warehouse(int id, int x, int y) : id(id), x(x), y(y){};
    ~Warehouse(){};
};

class Server {
   private:
    string frontHostName;
    string frontPortNum;
    string worldHostName;
    string worldPortNum;
    string upsHostName;
    string upsPortNum;
    int num_wh;
    int wh_distance;
    int worldID;
    vector<Warehouse> whlist;
    Threadpool threadPool;
    // global sequence number
    long seqNum;
    int front_fd;
    int ups_fd;
    int world_fd;
    /*To do:
        A map of sequence number and timer(and info of package?) to handle ack
       and resend
    */
   public:
    Server();
    ~Server();
    void run();
    template <bool withUPS>
    void connectWorld();
    void connectUPS();
    void connectWeb();
    void connectDB();
};

#endif
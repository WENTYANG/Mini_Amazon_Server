#ifndef _SERVER_H
#define _SERVER_H

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include <mutex>
#include "./protobuf/AUprotocolV3.pb.h"
#include "./protobuf/world_amazon.pb.h"
#include "ThreadSafe_queue.h"
#include "exception.h"
#include "proto.h"
#include "sql_functions.h"
#include "threadpool.h"
#include "warehouse.h"

using namespace std;
using namespace pqxx;

class Server {
   private:
    string frontHostName;
    string frontPortNum;
    string worldHostName;
    string worldPortNum;
    string upsHostName;
    string upsPortNum;
    int worldID;
    // global sequence number
    long seqNum;
    std::mutex seqnum_mtx;

   public:
    int num_wh;
    vector<unique_ptr<Warehouse>> whlist;
    vector<Product> productList;

    Threadpool threadPoolObj;
    Threadpool* threadPool;

    int ups_fd;
    int world_fd;
    int frontend_fd;
    proto_in* world_in;
    proto_out* world_out;
    proto_in* ups_in;
    proto_out* ups_out;

    ThreadSafe_queue<ACommands> world_output_queue;
    ThreadSafe_queue<ACommands> ups_output_queue;

    // db configure
    string dbName;
    string userName;
    string password;

    // Developing mode
    bool withUPS;
    bool withFrontEnd;

    /*To do:
        A map of sequence number and timer(and info of package?) to handle ack
       and resend
    */

   private:
    Server();

   public:
    ~Server();
    Server(const Server&) = delete;
    Server& operator=(const Server) = delete;
    static Server& get_instance() {
        static Server instance;
        return instance;
    }

    void run();
    void connectWorld();
    void connectUPS();
    void acceptOrder();
    connection* connectDB();
    void disConnectDB(connection* C);

    // Developing & testing functions
    void setWh_circle(AConnect& acon);

    int getSeqNum() {
        std::lock_guard<std::mutex> server_lk(seqnum_mtx);
        return ++seqNum;
    }
};

#endif

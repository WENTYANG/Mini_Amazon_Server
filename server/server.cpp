#include "server.h"
#include <cmath>
#include "proto.h"
#include "socket.h"

/*-----------------------------Server
 * constructor-----------------------------------*/
Server::Server()
    : frontHostName("0.0.0.0"),
      frontPortNum("8888"),
      worldHostName("vcm-24273.vm.duke.edu"),
      worldPortNum("23456"),
      upsHostName("0.0.0.0"),
      upsPortNum("8888"),
      num_wh(5),
      wh_distance(100),
      seqNum(0) {
    cout << "Initializing server configuration...." << endl;
    /*
    To do: initialize threadpool
    Uninitialized: worldID
    */
}
/*-----------------------------Server run-----------------------------------*/
void Server::run() {
    // Connect to UPS, receive world ID
    // connectUPS();

    // Connect to world, when developing, set withUPS=false to initialize a new
    // world
    connectWorld<false>();
    // Connect to front end
    connectWeb();
    // Connect to Database
    connectDB();
}

/*
  Connect to UPS and receive worldID
*/
void Server::connectUPS() {
    ups_fd = clientRequestConnection(upsHostName, upsPortNum);
    string world_id = socketRecvMsg(ups_fd);

    worldID = stoi(world_id);
    cout << "Receiving world id = " << worldID << " from UPS" << endl;
}

/*
  Connect to world server.
  When withUPS=false, init a new world, else connect using the worldID.
*/
template <bool withUPS>
void Server::connectWorld() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;  // use macro to check environment

    // connect to world
    world_fd = clientRequestConnection(worldHostName, worldPortNum);

    // build AConnect Command
    AConnect acon;
    if (withUPS) {
        acon.set_worldid(worldID);
    }
    /*
    Initialize Warehouses, one on center, others evenly distributed on circle
      x = r*sin(theta)
      y = r*cos(theta)
    */
    for (int i = 0; i < num_wh; i++) {
        AInitWarehouse* wh = acon.add_initwh();
        wh->set_id(i);
        if (i == 0) {
            wh->set_x(0);
            wh->set_y(0);
            whlist.push_back(Warehouse(i, 0, 0));
        } else {
            double theta = (2 * PI / num_wh) * i;
            int x = wh_distance * sin(theta);
            int y = wh_distance * cos(theta);
            wh->set_x(x);
            wh->set_y(y);
            whlist.push_back(Warehouse(i, x, y))
        }
    }
    // send AConnect Command
    unique_ptr<proto_out> out(new proto_out(world_fd));
    if (sendMesgTo<AConnect>(acon, out.get()) == false) {
        throw MyException("Send AConnect failed.");
    }

    // Receive Aconnected Response
    AConnected aced;
    unique_ptr<proto_in> in(new proto_in(world_fd));
    if (recvMesgFrom<AConnected>(aced, in.get()) == false) {
        throw MyException("Receive AConnected failed.");
    }

    // Verify Aconnected Response
    if (aced.result() != "connected!") {
        throw MyException("Connect to world failed.");
    }
    worldID = aced.worldid();
    cout << "Connect to world successfully." << endl;
}
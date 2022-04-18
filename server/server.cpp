#include "server.h"
#include <cmath>
#include <thread>
#include "proto.h"
#include "socket.h"
#include "sql_functions.h"

using namespace std;

/*-----------------------------Server constructor-----------------------------*/
Server::Server()
    : frontHostName("0.0.0.0"),
      frontPortNum("8888"),
      worldHostName("vcm-24273.vm.duke.edu"),
      worldPortNum("12345"),
      upsHostName("0.0.0.0"),
      upsPortNum("8888"),
      wh_distance(100),
      seqNum(0),
      dbName("MINI_AMAZON"),
      userName("postgres"),
      password("passw0rd"),
      withUPS(false),
      withFrontEnd(false) {
    cout << "Initializing server configuration...." << endl;

    // Initialize threadpool
    Threadpool thread_pool;
    threadPool = thread_pool.get_pool();

    if (withFrontEnd) {
        // Get warehouse amount and list
        initFromDB();
    } else {
        num_wh = 5;
    }
}

Server::~Server() {
    close(ups_fd);
    close(world_fd);
}

/*-----------------------------Server run-----------------------------------*/
void Server::run() {
    try {
        // Connect to UPS, receive world ID
        // connectUPS();

        // Read warehouse locations from DB
        initFromDB();

        // Connect to world, when developing, set withUPS=false to initialize a
        // new world
        connectWorld();

        thread t_RecvFromUps(RecvFromUps, this);
        thread t_RecvFromWorld(RecvFromWorld, this);

        t_RecvFromUps.detach();
        t_RecvFromWorld.detach();

        // Accept order from front end
        acceptOrder();
    } catch (const std::exception& e) {
        std::cerr << e.what() << endl;
    }
}

/*
  Connect to UPS and receive worldID
*/
void Server::connectUPS() {
    ups_fd = clientRequestConnection(upsHostName, upsPortNum);
    /*
        To be modified
    */
    string world_id = socketRecvMsg(ups_fd);

    worldID = stoi(world_id);
    cout << "Receiving world id = " << worldID << " from UPS" << endl;
}

/*
  Connect to world server.
  When withUPS=false, init a new world, else connect using the worldID.
*/
void Server::connectWorld() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;  // use macro to check environment

    // connect to world
    try {
        world_fd = clientRequestConnection(worldHostName, worldPortNum);
    } catch (const std::exception& e) {
        std::cerr << "Fail to connect to world because " << e.what() << endl;
    }

    // build AConnect Command
    AConnect acon;
    if (withUPS) {
        acon.set_worldid(worldID);
    } else {
        // acon.set_worldid(1);
    }

    // Initialize warehouses
    if (withFrontEnd) {
    } else {
        setWh_circle(acon);
    }

    acon.set_isamazon(true);

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
        string description = "Connect to world failed " + aced.result();
        throw MyException(description);
    }
    worldID = aced.worldid();
    cout << "Connect to world with world ID=" << worldID << " successfully."
         << endl;
}

/*
    Connect to database
*/
connection* Server::connectDB() {
    connection* C =
        new connection("host=db port=5432 dbname=" + dbName +
                       " user=" + userName + " password=" + password);
    if (C->is_open()) {
        cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
        throw MyException("Can't open database.");
    }
    return C;
}

/*
  close the connection to database.
*/
void Server::disConnectDB(connection* C) {
    C->disconnect();
}

/*
    Keep receiving connection from front end, and receive an int as order number
*/
void Server::acceptOrder() {
    // Listen on port for front end connection
    int server_fd = initializeServer(frontPortNum);
    // Continuously receiving connection from front end
    while (1) {
        int front_fd;
        string clientIP;
        try {
            front_fd = serverAcceptConnection(server_fd, clientIP);
            string request = socketRecvMsg(front_fd);
            close(front_fd);
        } catch (const std::exception& e) {
            std::cerr << e.what() << endl;
            continue;
        }
    }
}

/*--------------------------Testing & Developing funcs---------------*/

/*
    Initialize Warehouses, one on center, others evenly distributed on circle
      x = r*sin(theta)
      y = r*cos(theta)
*/
void Server::setWh_circle(AConnect& acon) {
    for (int i = 0; i < num_wh; i++) {
        AInitWarehouse* wh = acon.add_initwh();
        wh->set_id(i);
        if (i == 0) {
            wh->set_x(0);
            wh->set_y(0);
            whlist.push_back(Warehouse(i, 0, 0));
        } else {
            double theta = (2 * 3.14159 / num_wh) * i;
            int x = wh_distance * sin(theta);
            int y = wh_distance * cos(theta);
            wh->set_x(x);
            wh->set_y(y);
            whlist.push_back(Warehouse(i, x, y));
        }
    }
}

void Server::RecvFromUps() {
    unique_ptr<proto_in> ups_in(new proto_in(ups_fd));
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

void Server::RecvFromWorld() {
    unique_ptr<proto_in> world_in(new proto_in(world_fd));
    while (1) {
        try {
            AResponses response;
            if (recvMesgFrom<AResponses>(response, world_in.get()) == false) {
                throw MyException(
                    "Error occured when receiving AUResponse from UPS");
            }
            // Parse AResponses
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
}
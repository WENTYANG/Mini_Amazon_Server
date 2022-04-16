#include "server.h"

/*-----------------------------Server
 * constructor-----------------------------------*/
Server::Server()
    : frontHostName("0.0.0.0"),
      frontPortNum("1234"),
      worldHostName("vcm-24273.vm.duke.edu"),
      worldPortNum("12345"),
      upsHostName("0.0.0.0"),
      upsPortNum("8888"),
      num_wh(1),
      seqNum(0) {
    cout << "Initializing server configuration...." << endl;
    /*
    To do: initialize threadpool
    Uninitialized: worldID
    */
}
/*-----------------------------Server run-----------------------------------*/
void Server::run() {
    // Connect to
    // Initialize a world (for developing)
}
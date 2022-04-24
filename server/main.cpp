#include <string>
#include "server.h"

int main(int argc, char **  argv) {
    if (argc != 2) {
      std::cout << "Please input the UPS host name\n";
      exit(EXIT_FAILURE);
    }
    string ups_host = string(argv[1]);
    Server& s = Server::get_instance();
    s.run(ups_host);
    return 0;
}

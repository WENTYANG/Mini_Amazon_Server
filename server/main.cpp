#include "server.h"

int main() {
    Server& s = Server::get_instance();
    s.run();
    return 0;
}
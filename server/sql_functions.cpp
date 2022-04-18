#include "sql_functions.h"
#include "server.h"

/*
    Connect to Database and read warehouse amount and locations from Database
*/
void initFromDB() {
    Server& s = Server::get_instance();
    unique_ptr<connection> C(s.connectDB());
    nontransaction N(*C.get());
    stringstream sql;
    sql << "SELECT w_id, loc_x, loc_y FROM " << WAREHOUSE << ";";
    result warehouses(N.exec(sql.str()));
    if (warehouses.empty()) {
        throw MyException("No warehouses in database");
    }
    s.num_wh = warehouses.capacity();
    for (auto const& wh : warehouses) {
        int w_id = wh[0].as<int>();
        int loc_x = wh[1].as<int>();
        int loc_y = wh[2].as<int>();
        s.whlist.push_back(Warehouse(w_id, loc_x, loc_y));
    }
    s.disConnectDB(C.get());
}
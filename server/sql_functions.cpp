#include "sql_functions.h"
#include "server.h"
#include "warehouse.h"

/*
    1) read product from Database, store in Warehouse::productList
    2) read warehouse amount and locations from Database

*/
void initFromDB() {
    Server& s = Server::get_instance();
    unique_ptr<connection> C(s.connectDB());
    nontransaction N(*C.get());
    stringstream sql;

    // initialize Warehouse::productList
    sql << "SELECT p_id, name FROM " << PRODUCT << ";";
    result products(N.exec(sql.str()));
    if (products.empty()) {
        throw MyException("No products in database");
    }

    sql.clear();
    sql.str("");

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
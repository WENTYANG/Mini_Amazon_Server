#include "sql_functions.h"
#include "server.h"
#include "warehouse.h"

/*
    1) read product from Database, store in Warehouse::productList
    2) read warehouse amount and locations from Database
    If any of them is initialized(db empty), throw Uninitialize exception
*/
void initFromDB() {
    Server& s = Server::get_instance();
    unique_ptr<connection> C(s.connectDB());
    nontransaction N(*C.get());
    stringstream sql;

    // Initialize Server.productList
    sql << "SELECT p_id, name FROM " << PRODUCT << ";";
    result products(N.exec(sql.str()));
    if (products.empty()) {
        throw Uninitialize("No products in database.");
    }
    for (auto const& p : products) {
        int p_id = p[0].as<int>();
        string p_name = p[1].as<string>();
        Server::get_instance().productList.push_back(Product(p_id, p_name));
    }

    sql.clear();
    sql.str("");

    // Initialize Server.whlist
    sql << "SELECT w_id, loc_x, loc_y FROM " << WAREHOUSE << ";";
    result warehouses(N.exec(sql.str()));
    s.disConnectDB(C.get());
    if (warehouses.empty()) {
        throw Uninitialize("No warehouses in database.");
    }
    s.num_wh = warehouses.capacity();
    for (auto const& wh : warehouses) {
        int w_id = wh[0].as<int>();
        int loc_x = wh[1].as<int>();
        int loc_y = wh[2].as<int>();
        s.whlist.push_back(Warehouse(w_id, loc_x, loc_y));
    }
}

/*
    Check inventory in DB, if sufficient for order, reserve products
   simultaneously and return true return false if insufficient
*/
bool checkInventory(int w_id, int p_id, int purchase_amount) {
    Server& s = Server::get_instance();
    unique_ptr<connection> C(s.connectDB());

    work W(*C.get());
    stringstream sql;

    sql << "UPDATE " << INVENTORY << "," << PRODUCT << "," << WAREHOUSE
        << " SET count=" << INVENTORY << ".count-" << purchase_amount
        << " WHERE " << INVENTORY << ".product=" << PRODUCT << ".p_id AND "
        << INVENTORY << ".warehouse=" << WAREHOUSE << ".w_id AND" << INVENTORY
        << ".count>=" << purchase_amount;

    result R(W.exec(sql.str()));
    s.disConnectDB(C.get());
    result::size_type rows = R.affected_rows();
    if (rows == 0) {
        return false;
    }
    return true;
}

/*
    Given an order id, read the order info from DB
*/
void readOrder(int o_id, Product& product, int& purchase_amount) {
    Server& s = Server::get_instance();
    unique_ptr<connection> C(s.connectDB());

    nontransaction N(*C.get());
    stringstream sql;

    sql << "SELECT p_id, name, count FROM " << ITEM << ", " << PRODUCT
        << " WHERE " << ITEM << ".i_id=" << o_id << " AND " << ITEM
        << ".PRODUCT=" << PRODUCT << "."
        << "p_id"
        << ";";
    result R(N.exec(sql.str()));

    if (R.capacity() == 0) {
        throw MyException("Order id (item id) does not exist in database.");
    }

    product.p_id = R[0][0].as<int>();
    product.name = R[0][1].as<string>();
    purchase_amount = R[0][2].as<int>();
}

// add inventory to specific warehouse
void add_inventory(int w_id, int p_id, int count) {
}

// change order status to packed and return if ups truck has arrived
// throw if current order status is not open
bool packed_and_check_ups_truck(int i_id) {
}

// change order status to delivering
// throw if current order status is not packed
void change_status_to_delivering(int i_id) {
}

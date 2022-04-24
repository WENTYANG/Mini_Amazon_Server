#ifndef _SQL_FUNCTION_H
#define _SQL_FUNCTION_H

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include "exception.h"

/*------------------Table Names-------------*/
#define PRODUCT "amazon_product"
#define INVENTORY "amazon_inventory"
#define CATEGORY "amazon_category"
#define ITEM "amazon_item"
#define ORDER "amazon_order"
#define WAREHOUSE "amazon_warehouse"
#define USER "auth_user"
#define CUSTOMER "user_customer"

using namespace std;
using namespace pqxx;

void initFromDB();
bool checkInventory(int w_id, int p_id, int purchase_amount);
void readOrder(int o_id);

void add_inventory(int w_id, int p_id, int count);
tuple<bool, int, int> packed_and_check_ups_truck(int i_id);
void change_status_to_delivering(int i_id);

pair<bool, int> arrived_and_check_if_packed(int i_id, int truck_id);
void change_status_to_delivered(int i_id);
string get_email_addr(int i_id);
#endif

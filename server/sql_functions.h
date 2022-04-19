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

using namespace std;
using namespace pqxx;

class Product;

void initFromDB();
bool checkInventory(int w_id, int p_id, int purchase_amount);
void readOrder(int o_id);
void add_inventory(int w_id, int p_id, int count);
bool packed_and_check_ups_truck(int i_id);
void change_status_to_delivering(int i_id);

#endif

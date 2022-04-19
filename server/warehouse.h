#ifndef _WAREHOUSE_H
#define _WAREHOUSE_H

#include <queue>
#include <unordered_map>
#include <vector>
#include "server.h"
#include "sql_functions.h"

using namespace std;
class SubOrder;
typedef queue<SubOrder*> purchaseQueue;
typedef int p_id_t;

/*
int32_t id() const;
int32_t x() const;
int32_t y() const;
*/

class Product {
   public:
    int p_id;
    string name;

    Product() {}
    Product(int id, string name) : p_id(id), name(name) {}
    ~Product() {}
};

/*SubOrder对应数据库里的ITEM*/
class SubOrder {
   public:
    int o_id;
    Product product;
    int purchase_amount;
    int loc_x;
    int loc_y;

   public:
    SubOrder() {}
    SubOrder(int o_id, int p_id, string name, int purchase_amount, int x, int y)
        : o_id(o_id),
          product(Product(p_id, name)),
          purchase_amount(purchase_amount),
          loc_x(x),
          loc_y(y) {}
    ~SubOrder() {}
};

class Warehouse {
   public:
    int w_id;
    int x;
    int y;

    unordered_map<p_id_t, purchaseQueue*> productMap;

    // purchaseQueue

    Warehouse(int id, int x, int y) : w_id(id), x(x), y(y) {
        // Initialize product queues for every warehouse
        for (auto& p : Server::get_instance().productList) {
            purchaseQueue* q = new queue<SubOrder*>();
            productMap[p.p_id] = q;
        }
    };
    ~Warehouse(){};
};

void checkOrder(int w_id);
void purchaseMore(int w_id, int p_id, int amount);
int selectWarehouse(int loc_x, int loc_y);
void pushInQueue(int wh_index, SubOrder* order);

#endif
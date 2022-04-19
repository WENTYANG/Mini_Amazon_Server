#ifndef _WAREHOUSE_H
#define _WAREHOUSE_H

#include <queue>
#include <unordered_map>
#include <vector>
#include "server.h"
#include "sql_functions.h"

using namespace std;
typedef int o_id_t;
typedef queue<o_id_t> purchaseQueue;
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

class Order {
   public:
    int o_id;
    Product product;
    int purchase_amount;

   public:
    Order(int o_id) : o_id(o_id) { readOrder(o_id, product, purchase_amount); }
    ~Order() {}
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
            purchaseQueue* q = new queue<int>();
            productMap[p.p_id] = q;
        }
    };
    ~Warehouse(){};
};

void checkOrder(int w_id);
void purchaseMore(int w_id, int p_id, int amount);

#endif
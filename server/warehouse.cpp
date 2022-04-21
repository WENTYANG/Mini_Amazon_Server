#include "warehouse.h"
#include <cmath>
#include "server.h"
#include "sql_functions.h"
#include "working_threads.h"

#define QUANTUM 5
#define IS_PURCHASING true
#define REGULAR_PURCHASE_AMOUNT 10000
using namespace std;

Warehouse::Warehouse(int id, int x, int y) : w_id(id), x(x), y(y) {
    // Initialize product queues for every warehouse
    Server& s = Server::get_instance();
    for (auto& p : s.productList) {
        purchaseQueue* q = new queue<shared_ptr<SubOrder>>();
        productMap[p.p_id] = q;
    }
}

/*
    For a warehouse, check orders for all the products in a round robin fashion
*/
void checkOrder(int w_idx) {
    Server& s = Server::get_instance();
    unique_ptr<Warehouse>& w = s.whlist[w_idx];
    int w_id = s.whlist[w_idx]->w_id;
    int num_product = s.productList.size();
    vector<bool> isPurchasing(num_product, IS_PURCHASING);

    // Round Robin between every product in the warehouse
    while (1) {
        for (int i = 0; i < num_product; i++) {
            for (int time = 0; time < QUANTUM; time++) {
                int p_id = s.productList[i].p_id;
                string name = s.productList[i].name;
                purchaseQueue* q = w->productMap[p_id];
                if (q->empty()) {
                    //当前没有order，去下一个queue
                    break;
                }
                // Check inventory in DB
                shared_ptr<SubOrder> order = q->front();
                if (checkInventory(w_id, p_id, order->purchase_amount)) {
                    // Sufficient, if just purchased, update ispurchasing flag
                    isPurchasing[p_id] = false;
                    q->pop();
                    // Spawn a task to pack
                    s.threadPool->assign_task(bind(pack, order, w_id));
                } else {
                    // Insufficient
                    if (!isPurchasing[p_id]) {
                        // Send purchaseMore to world
                        purchaseMore(w_id, p_id, name, order->purchase_amount);
                        isPurchasing[p_id] = true;
                    }
                    //库存不够，刚刚发送了purchase，去下一个queue
                    break;
                }
            }
        }
    }
}

/*
    Send APurchaseMore Command to World
*/
void purchaseMore(int w_id, int p_id, string name, int amount) {
    // Initialize Products
    Server& s = Server::get_instance();
    ACommands cmd;
    APurchaseMore* purchaseMore = cmd.add_buy();
    purchaseMore->set_whnum(w_id);
    purchaseMore->set_seqnum(s.getSeqNum());
    AProduct* initProduct = purchaseMore->add_things();
    initProduct->set_id(p_id);
    initProduct->set_description(name);
    initProduct->set_count(REGULAR_PURCHASE_AMOUNT);

    // Send initialize Product command-->push into world queue
    s.world_output_queue.push(cmd);
}

/*
    Given a location of order, select the nearest warehouse, return the
   index of wh in whlist
*/
int selectWarehouse(int loc_x, int loc_y) {
    int min_dist = INT_MAX;
    int index = -1;
    int i = 0;
    for (auto const& w : Server::get_instance().whlist) {
        int dist = pow(w->x - loc_x, 2) + pow(w->y - loc_y, 2);
        if (min_dist > dist) {
            min_dist = dist;
            index = i;
        }
        i++;
    }
    return index;
}

/*
    Given a warehouse index in the whlist, push the order into the corresponding
   purchaseQueue
*/
void pushInQueue(int wh_index, shared_ptr<SubOrder> order) {
    Server& s = Server::get_instance();
    unique_ptr<Warehouse>& wh = s.whlist[wh_index];
    purchaseQueue* q = wh->productMap[order->product.p_id];
    q->push(order);
}

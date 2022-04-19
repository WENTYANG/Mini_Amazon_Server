#include "warehouse.h"
#include "server.h"
#include "sql_functions.h"

#define QUANTUM 5

void checkOrder(int w_id) {
    Server& s = Server::get_instance();
    Warehouse& w = s.whlist[w_id];
    int num_product = s.productList.size();
    vector<bool> isPurchasing(num_product, false);

    // Round Robin between every product in the warehouse
    while (1) {
        for (int i = 0; i < num_product; i++) {
            for (int time = 0; time < QUANTUM; time++) {
                int p_id = s.productList[i].p_id;
                purchaseQueue* q = w.productMap[p_id];
                if (q->empty()) {
                    //当前没有order，去下一个queue
                    break;
                }
                // Check inventory in DB
                int o_id = q->front();
                Order order(o_id);
                if (checkInventory(p_id, w_id, order.purchase_amount)) {
                    // Sufficient, if just purchased, update ispurchasing flag
                    isPurchasing[p_id] = false;
                    // Spawn a task to pack
                    //  s.threadPool->assign_task(bind(pack, ...args));
                } else {
                    // Insufficient
                    if (!isPurchasing[p_id]) {
                        // Send purchaseMore to world
                        purchaseMore(w_id, p_id, order.purchase_amount);
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
void purchaseMore(int w_id, int p_id, int amount) {}
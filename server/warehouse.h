#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;
typedef queue<int> purchaseQueue;
// typedef string name;

/*
int32_t id() const;
int32_t x() const;
int32_t y() const;
*/

class Warehouse {
   public:
    int id;
    int x;
    int y;

    static vector<string> productList;

    unordered_map<string, purchase_queue*> productMap;

    // purchaseQueue

    Warehouse(int id, int x, int y) : id(id), x(x), y(y) {
        // Initialize product queues for every warehouse
        for (auto& productName : productList) {
            purchaseQueue* q = new queue<int>;
            productMap[productName] = q;
        }
    };
    ~Warehouse(){};
};
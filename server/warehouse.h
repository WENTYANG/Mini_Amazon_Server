#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;
typedef queue<int> purchaseQueue;
typedef int p_id;

/*
int32_t id() const;
int32_t x() const;
int32_t y() const;
*/

class Product {
   public:
    int id;
    string name;

    Product(int id, string name) : id(id), name(name) {}
    ~Product() {}
};

class Warehouse {
   public:
    int id;
    int x;
    int y;

    static vector<Product> productList;

    unordered_map<p_id, purchaseQueue*> productMap;

    // purchaseQueue

    Warehouse(int id, int x, int y) : id(id), x(x), y(y) {
        // Initialize product queues for every warehouse
        for (auto& p : productList) {
            purchaseQueue* q = new queue<int>;
            productMap[p.id] = q;
        }
    };
    ~Warehouse(){};
};
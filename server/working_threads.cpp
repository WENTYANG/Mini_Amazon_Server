#include "working_threads.h"
#include "server.h"

using namespace std;

//be ware of lifetime of lock
void sendAck(AResponses response) {
  ACommands cmd;
  for (int i = 0; i < response.arrived_size(); ++i) {
    int64_t seq = response.arrived(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.ready_size(); ++i) {
    int64_t seq = response.ready(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.loaded_size(); ++i) {
    int64_t seq = response.loaded(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.error_size(); ++i) {
    int64_t seq = response.error(i).seqnum();
    cmd.add_acks(seq);
  }
  for (int i = 0; i < response.packagestatus_size(); ++i) {
    int64_t seq = response.packagestatus(i).seqnum();
    cmd.add_acks(seq);
  }
  Server& s = Server::get_instance();
  s.world_output_queue.push(cmd); 
}

void purchase_more(APurchaseMore apm) {
  int whnum = apm.whnum();
  for (int j = 0; j < apm.things_size(); ++j) {
    const AProduct& product = apm.things(j);
    add_inventory(whnum, (int)product.id(), product.count());
  }
}

void load(APacked aped) {
    tuple<bool, int, int> info;
    try {
      // tuple(res, truck_id, w_id);
      info = packed_and_check_ups_truck(aped.shipid());
    } catch (MyException& e) {
      cout << e.what() << endl;
      return;
    }
    if (get<0>(info)) {
      Server& s = Server::get_instance();
      int i_id = aped.shipid();
      ACommands cmd;
      auto load = cmd.add_load();
      load->set_whnum(get<2>(info));
      load->set_truckid(get<1>(info));
      load->set_shipid(i_id);
      load->set_seqnum(s.getSeqNum());
      s.world_output_queue.push(cmd); 
    }
}

void ready_to_deliver(ALoaded aled) {
  try {
    change_status_to_delivering(aled.shipid());
  } catch (MyException& e) {
    cout << e.what() << endl;
    return;
  }
    Server& s = Server::get_instance();
    int i_id = aled.shipid();
    AUCommand cmd;
    auto deliver = cmd.add_deliver();
    deliver->set_packageid(i_id);
    deliver->set_seqnum(s.getSeqNum());
    s.ups_output_queue.push(cmd);
}

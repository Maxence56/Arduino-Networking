#ifndef ARD_NET_ROOT_ARD_NETWORK_LAYER_THREE_ROUTING_H
#define ARD_NET_ROOT_ARD_NETWORK_LAYER_THREE_ROUTING_H

#include "ard_network_address.h"
#include "ard_network_layer_base.h"

class ArdNetworkRoutingLayerThree : public ArdNetworkRoutingLayer<L3Addr>,
                                    public ArdNetSouthInterface {
public:
  ArdNetworkRoutingLayerThree(PktMemPool *a_mem_pool,
                              ArdSysInterface *a_ard_sys_int,
                              ArdEventManager *a_event_manager,
                              SendInterface<L3Addr> *a_send_interface_left,
                              SendInterface<L3Addr> *a_send_interface_right,
                              L3Addr a_this_addr_left, L3Addr
                              a_this_addr_right);
  void onDataReceived(PktBufPtr a_p, AnyAddr a_src_addr,
                      LayerId a_l_id) override;
  void dataHandlingDone(PktBufPtr a_p, bool res) override ;

};



#endif // ARD_NET_ROOT_ARD_NETWORK_LAYER_THREE_ROUTING_H

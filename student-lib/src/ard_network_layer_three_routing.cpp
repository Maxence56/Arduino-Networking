#include "ard_network_layer_three_routing.h"
#include "ard_network_layer_three_message.h"

ArdNetworkRoutingLayerThree::ArdNetworkRoutingLayerThree(
    PktMemPool *a_mem_pool, ArdSysInterface *a_ard_sys_int,
    ArdEventManager *a_event_manager,
    SendInterface<L3Addr> *a_send_interface_left,
    SendInterface<L3Addr> *a_send_interface_right,
    L3Addr a_this_addr_left,
    L3Addr a_this_addr_right)
    : ArdNetworkRoutingLayer(a_mem_pool, a_ard_sys_int, a_event_manager,
                             a_send_interface_left, a_send_interface_right,
                             a_this_addr_left,
                             a_this_addr_right) {}

void ArdNetworkRoutingLayerThree::onDataReceived(PktBufPtr a_p,
                                                 AnyAddr a_src_addr,
                                                 LayerId a_l_id)
{
}

void ArdNetworkRoutingLayerThree::dataHandlingDone(PktBufPtr a_p, bool res) {}


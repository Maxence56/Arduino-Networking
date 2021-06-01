#include "ard_network_layer_three.h"
#include "ard_network_layer_base.h"
#include "ard_network_layer_three_message.h"
#include "ard_utils.h"

/***********************************************************************
 *                        ArdNetworkLayerThree                         *
 **********************************************************************/
ArdNetworkLayerThree::ArdNetworkLayerThree(
    PktMemPool *a_mem_pool, ArdSysInterface *a_ard_sys_int,
    ArdEventManager *a_event_manager, SendInterface<L3Addr> *a_send_interface,
    L3Addr a_this_addr)
    : ArdNetworkLayer(a_mem_pool, a_ard_sys_int, a_event_manager,
                      a_send_interface, a_this_addr) {}

void ArdNetworkLayerThree::sendRequest(PktBufPtr a_p, L3Addr a_l3_dst_addr) {
}

void ArdNetworkLayerThree::dataHandlingDone(PktBufPtr, bool) {
}

void ArdNetworkLayerThree::onDataReceived(PktBufPtr a_p, AnyAddr a_src_addr,
                                          LayerId a_l_id) {
}

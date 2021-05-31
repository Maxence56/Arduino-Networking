
#ifndef ARD_NETWORK_LAYER_THREE_H
#define ARD_NETWORK_LAYER_THREE_H

#include "ard_circular_buffer.h"
#include "ard_event_manager.h"
#include "ard_mempool.h"
#include "ard_network_address.h"
#include "ard_network_layer_base.h"
#include "ard_pkt_buffer.h"
#include "ard_sys_interface.h"

class ArdNetworkLayerThree : public ArdNetworkLayer<L3Addr>,
                             public ArdNetSouthInterface,
                             public ArdNetNorthInterface<L3Addr> {
public:
  explicit ArdNetworkLayerThree(PktMemPool *a_mem_pool,
                                ArdSysInterface *a_ard_sys_int,
                                ArdEventManager *a_event_manager,
                                SendInterface<L3Addr> *a_send_interface,
                                L3Addr a_this_addr);
  void sendRequest(PktBufPtr, L3Addr) override;
  void onDataReceived(PktBufPtr, AnyAddr, LayerId) override;
  void dataHandlingDone(PktBufPtr, bool) override;
};
#endif // ARD_NETWORK_LAYER_THREE_H

#ifndef ARD_NETWORK_LAYER_TWO_FUNCTOR_H
#define ARD_NETWORK_LAYER_TWO_FUNCTOR_H

#include "ard_callbacks.h"

/** \addtogroup most_important
 *  @{
 */

class ArdNetworkLayerTwoAck;
typedef void (ArdNetworkLayerTwoAck::*ArdNetworkLayerTwoAckSend)(
    ArdUniquePtrMemPool<ArdPktBuffer>);

class SendPktBufferLTwoAckFunc : public TimerFunctor {
public:
  SendPktBufferLTwoAckFunc(ArdNetworkLayerTwoAck &a_net_layer,
                           ArdNetworkLayerTwoAckSend a_send_fun,
                           ArdMemPool<ArdPktBuffer> *a_mpool);
  void operator()() override;
  void setPktBuff(PktBufPtr a_ptr);

private:
  ArdNetworkLayerTwoAck &m_net_layer;
  ArdNetworkLayerTwoAckSend m_send_fun;
  ArdUniquePtrMemPool<ArdPktBuffer> m_pkt_buf_ptr;
};

/** @}*/

#endif // ARD_NETWORK_LAYER_TWO_FUNCTOR_H

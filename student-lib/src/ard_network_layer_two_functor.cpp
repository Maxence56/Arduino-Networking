#include "ard_network_layer_two_functor.h"
SendPktBufferLTwoAckFunc::SendPktBufferLTwoAckFunc(
    ArdNetworkLayerTwoAck &a_net_layer, ArdNetworkLayerTwoAckSend a_send_fun,
    ArdMemPool<ArdPktBuffer> *a_mpool)
    : m_net_layer(a_net_layer), m_send_fun(a_send_fun), m_pkt_buf_ptr(a_mpool) {
}

void SendPktBufferLTwoAckFunc::setPktBuff(PktBufPtr a_ptr) {
  m_pkt_buf_ptr = ard_move(a_ptr);
}

void SendPktBufferLTwoAckFunc::operator()() {
  if (m_pkt_buf_ptr) {
    CALL_MEMBER_FN(m_net_layer, m_send_fun)
    (ard_move(m_pkt_buf_ptr));
  } else {
    ard_error(ARD_F("Layer2Functor operator() pkt_buff_ptr not valid"));
  }
}

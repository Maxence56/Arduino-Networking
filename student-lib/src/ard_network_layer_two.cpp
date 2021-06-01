//
// Created by alberto on 30/05/19.
//

#include "ard_network_layer_two.h"
#include "ard_network_layer_base.h"
#include "ard_network_layer_two_message.h"
#include "ard_utils.h"

/***********************************************************************
 *                   THREE TXs                                         *
 **********************************************************************/

ArdNetworkLayerTwoThreeTxs::ArdNetworkLayerTwoThreeTxs(
    PktMemPool *a_mem_pool, ArdSysInterface *a_ard_sys_int,
    ArdEventManager *a_event_manager, SendInterface<L2Addr> *a_send_interface,
    L2Addr a_this_addr)
    : ArdNetworkLayer(a_mem_pool, a_ard_sys_int, a_event_manager,
                      a_send_interface, a_this_addr),
      m_send_seq_num(0), m_las_seq_num_rcv((uint8_t)-1), 
      m_queue(), m_sending(false),
      m_tx_count(0), m_last_seq_num_sent(0) {}

void ArdNetworkLayerTwoThreeTxs::sendRequest(PktBufPtr a_p,
                                             L2Addr a_l2_dst_addr)
{
  debug_pr(ARD_F("Layer 2 Send, size: "), int(a_p->curr_size), ARD_F("\n"));
  // We build an object of type L2Message, which contains all the fields of
  // the layer two header.
  L2Message l2_msg(m_this_addr, a_l2_dst_addr, a_p->curr_size, m_send_seq_num,
                   L2_TYP_DATA);

  // We use the serialize method of the L2Message object to build its binary
  // representation, including its payload, pointed by the a_p parameter. The
  // serialize method stores the packet buffer pointer in the m_pkt_buff
  // attribute of the l2_msg object.  The method returns the unique pointer
  // pointing to the payload. This way we can return it (pass the token) back to
  // the upper layer (see below).
  a_p = l2_msg.serialize(ard_move(a_p), m_mem_pool);

  // We increment the sequence number counter.  So that it will have the
  // correct value next time we need to send a packet.
  ++m_send_seq_num;

  // Before adding a packet to the transmission queue, we need to make sure
  // that it is not full.  If it is, we print a message and drop the packet.
  bool res = false;
  if (m_queue.isFull())
  {
    info_pr(ARD_F("L2: dropping a frame because the tx queue is full\n"));
    res = false;
  }
  else
  {
    res = true;
    // If the queue is not full, we can add a packet.  Note that we have to
    // use ard_move because we are relinquishing the ownership of the
    // corresponding memory buffer ("we are passing the token").
    m_queue.addElement(ard_move(l2_msg.m_pkt_buff));
    // Finally, we cal checkQueue to start the transmission, if the packet we
    // just added is the only one in the queue and if we are not waiting for
    // a previous transmission to finish.
    checkQueue();
  }
  if (m_upper_layer)
  {
    m_upper_layer->dataHandlingDone(ard_move(a_p), res);
  }
}


void ArdNetworkLayerTwoThreeTxs::checkQueue() {
  if (!m_sending && !m_queue.isEmpty()) {
    m_sending = true;
    PktBufPtr l2_p = m_queue.removeElement();
    // Store the last sequence number sent
    m_last_seq_num_sent = getSeqNumFromBuffer(l2_p->data);
    doSend(ard_move(l2_p));
  }
}

void ArdNetworkLayerTwoThreeTxs::doSend(PktBufPtr l2_p) {
  if (!(m_send_interface)) {
    ard_error(ARD_F("Layer 2 sendRequest: lower layer is not set\n "));
  }
  L2Addr dst_addr = L2Message::getDstAddr(l2_p->data);
  m_send_interface->sendRequest(ard_move(l2_p), dst_addr);
}

void ArdNetworkLayerTwoThreeTxs::dataHandlingDone(PktBufPtr l2_p, bool res) {
  m_sending = false;
  if (res) {
    ++m_tx_count;
  }
  if (m_tx_count < 3) {
    // We add the packet back in the queue
    m_queue.addElementToFront(ard_move(l2_p));
  } else {
    // we have transmitted this packet three times, we must reset the counter
    m_tx_count = 0;
  }
  // We call checkQueue to process any pending packet
  checkQueue();
}

void ArdNetworkLayerTwoThreeTxs::onDataReceived(PktBufPtr a_p,
                                                AnyAddr a_src_addr,
                                                LayerId a_l_id) {
  debug_pr(ARD_F("Layer 2 onDataReceived, size: "), int(a_p->curr_size),
           ARD_F("\n"));
  if (a_p->curr_size < 4) {
    info_pr(ARD_F("Layer 2 ignoring data frame, too small, size: "),
            int(a_p->curr_size), ARD_F("\n"));
    return;
  }
  L2Message l2_msg;
  PktBufPtr payload_p = l2_msg.deSerialize(ard_move(a_p), m_mem_pool);

  if (m_las_seq_num_rcv == l2_msg.m_seq_num){
      // we have already processed this message (this is a copy)
      return;
  } 
  m_las_seq_num_rcv = l2_msg.m_seq_num;

  if (!(m_upper_layer)) {
    ard_error(ARD_F("Layer 2 onDataReceived: north interface is not bound\n "));
  }
  m_upper_layer->onDataReceived(ard_move(payload_p), AnyAddr(l2_msg.m_src_add),
                                m_l_id);
}

uint8_t ArdNetworkLayerTwoThreeTxs::getSeqNumFromBuffer(uint8_t *p) {
  return p[L2Message::l2_seq_num_offset];
}

/***********************************************************************
 *                        ACKx                                         *
 **********************************************************************/

ArdNetworkLayerTwoAck::ArdNetworkLayerTwoAck(
    PktMemPool *a_mem_pool, ArdSysInterface *a_ard_sys_int,
    ArdEventManager *a_event_manager, SendInterface<L2Addr> *a_send_interface,
    L2Addr a_this_addr)
    : ArdNetworkLayer(a_mem_pool, a_ard_sys_int, a_event_manager,
                      a_send_interface, a_this_addr),
      m_send_seq_num(0), m_last_seq_num_processed(0xFF), m_queue(),
      m_sending(false), m_tx_count(0),
      m_last_seq_num_sent(0),
      m_timer_handler(a_event_manager, a_ard_sys_int),
      m_timer_func(*this, &ArdNetworkLayerTwoAck::doSend, a_mem_pool) {}

void ArdNetworkLayerTwoAck::sendRequest(PktBufPtr a_p, L2Addr a_l2_dst_addr) {
  debug_pr(ARD_F("Layer 2 Send, size: "), int(a_p->curr_size), ARD_F(", to "),
           a_l2_dst_addr.m_addr, ARD_F("\n"));
// We build an object of type L2Message, which contains all the fields of
  // the layer two header.
  L2Message l2_msg(m_this_addr, a_l2_dst_addr, a_p->curr_size, m_send_seq_num,
                   L2_TYP_DATA);

  // We use the serialize method of the L2Message object to obtain its binary
  // representation, including its payload, thanks to the a_p parameter
 a_p = l2_msg.serialize(ard_move(a_p), m_mem_pool);
  // We increment the sequence number counter.  So that it will have the
  // correct value next time we need to send a packet.
  ++m_send_seq_num;

  // Before adding a packet to the transmission queue, we need to make sure
  // that it is not full.  If it is, we print a message and drop the packet.
  bool res = false;
  if (m_queue.isFull()) {
    info_pr(ARD_F("L2: dropping a frame because the tx queue is full\n"));
    res = false;
  } else {
    res = true;
    // If the queue is not full, we can add a packet.  Note that we have to
    // use ard_move because we are relinquishing the ownership of the
    // corresponding memory buffer ("we are passing the token").
    m_queue.addElement(ard_move(l2_msg.m_pkt_buff));
    // Finally, we cal checkQueue to start the transmission, if the packet we
    // just added is the only one in the queue and if we are not waiting for
    // a previous transmission to finish.
    checkQueue();
  }
  if (m_upper_layer)
  {
    m_upper_layer->dataHandlingDone(ard_move(a_p), res);
  }
}

void ArdNetworkLayerTwoAck::checkQueue() {
  if (!m_sending && !m_queue.isEmpty()) {
    m_sending = true;
    PktBufPtr l2_p = m_queue.removeElement();
    // Store the last sequence number sent
    m_last_seq_num_sent = getSeqNumFromBuffer(l2_p->data);
    doSend(ard_move(l2_p));
  }
}

void ArdNetworkLayerTwoAck::doSend(PktBufPtr l2_p) {
  if (!(m_send_interface)) {
    ard_error(ARD_F("Layer 2 sendRequest: lower layer is not set\n "));
  }
  L2Addr dst_addr = L2Message::getDstAddr(l2_p->data);
  m_send_interface->sendRequest(ard_move(l2_p), dst_addr);
}

void ArdNetworkLayerTwoAck::dataHandlingDone(PktBufPtr l2_p, bool res) {
  m_timer_func.setPktBuff(ard_move(l2_p));
  m_timer_handler.startTimer(1000, &m_timer_func);
}

void ArdNetworkLayerTwoAck::onDataReceived(PktBufPtr a_p, AnyAddr a_src_addr,
                                           LayerId a_l_id) {
  debug_pr(ARD_F("Layer 2 onDataReceived, size: "), int(a_p->curr_size),
           ARD_F("\n"));
  if (a_p->curr_size < 4) {
    info_pr(ARD_F("Layer 2 ignoring data frame, too small, size: "),
            int(a_p->curr_size), ARD_F("\n"));
    return;
  }
  L2Message l2_msg;
  PktBufPtr payload_p = l2_msg.deSerialize(ard_move(a_p), m_mem_pool);

  if (!(m_upper_layer)) {
    ard_error(ARD_F("Layer 2 onDataReceived: north interface is not bound\n "));
  }
  m_upper_layer->onDataReceived(ard_move(payload_p), AnyAddr(l2_msg.m_src_add),
                                m_l_id);
  L2Message ack_msg( m_this_addr, l2_msg.m_src_add, 0, m_send_seq_num, L2_TYP_ACK);
  ArdUniquePtrMemPool<ArdPktBuffer> a_p1 = ack_msg.serialize(m_mem_pool);
  if (!(m_send_interface)) {
    ard_error(ARD_F("Layer 2 sendRequest: lower layer is not set\n "));
  }
  L2Addr dst_addr = l2_msg.m_src_add;
  m_send_interface->sendRequest(ard_move(a_p1), dst_addr);
}

uint8_t ArdNetworkLayerTwoAck::getSeqNumFromBuffer(uint8_t *p) {
  return p[L2Message::l2_seq_num_offset];
}

#ifndef ARD_NETWORK_LAYER_TWO_H
#define ARD_NETWORK_LAYER_TWO_H

#include "ard_circular_buffer.h"
#include "ard_event_manager.h"
#include "ard_mempool.h"
#include "ard_network_address.h"
#include "ard_network_layer_base.h"
#include "ard_network_layer_two_functor.h"
#include "ard_pkt_buffer.h"
#include "ard_sys_interface.h"
#include "ard_peer_context.h"

/** \addtogroup most_important 
 *  @{
 */

class ArdNetworkLayerTwoThreeTxs : public ArdNetworkLayer<L2Addr>,
                                   public ArdNetSouthInterface,
                                   public ArdNetNorthInterface<L2Addr> {
public:
  explicit ArdNetworkLayerTwoThreeTxs(PktMemPool *a_mem_pool,
                                      ArdSysInterface *a_ard_sys_int,
                                      ArdEventManager *a_event_manager,
                                      SendInterface<L2Addr> *a_send_interface,
                                      L2Addr a_this_addr);
  void sendRequest(PktBufPtr, L2Addr) override;
  void onDataReceived(PktBufPtr, AnyAddr, LayerId) override;
  void dataHandlingDone(PktBufPtr, bool) override;
  void checkQueue();
  void doSend(PktBufPtr);
  uint8_t getSeqNumFromBuffer(uint8_t *);

private:
  uint8_t m_send_seq_num;
  uint8_t m_las_seq_num_rcv;
  uint8_t m_last_seq_num_sent;
  ArdCircularBuffer<PktBufPtr> m_queue;
  bool m_sending;
  uint8_t m_tx_count;
};

class ArdNetworkLayerTwoAck : ArdNetworkLayer<L2Addr>,
                              public ArdNetSouthInterface,
                              public ArdNetNorthInterface<L2Addr> {
public:
  explicit ArdNetworkLayerTwoAck(PktMemPool *a_mem_pool,
                                 ArdSysInterface *a_ard_sys_int,
                                 ArdEventManager *a_event_manager,
                                 SendInterface<L2Addr> *a_send_interface,
                                 L2Addr a_this_addr);
  void sendRequest(PktBufPtr, L2Addr) override;
  void onDataReceived(PktBufPtr, AnyAddr, LayerId) override;
  void dataHandlingDone(PktBufPtr, bool) override;
  void checkQueue();
  void doSend(PktBufPtr);
  uint8_t getSeqNumFromBuffer(uint8_t *);

private:
  //uint8_t m_send_seq_num;
  //uint8_t m_last_seq_num_sent;
  //uint8_t m_last_seq_num_processed;
  ArdCircularBuffer<PktBufPtr> m_queue;
  bool m_sending;
  uint8_t m_tx_count;
  SendPktBufferLTwoAckFunc m_timer_func;
  ArdTimerEventHandler m_timer_handler;
  PeerContextPool m_contexts;
};

/** @}*/
#endif // ARD_NETWORK_LAYER_TWO_H

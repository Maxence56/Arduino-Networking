/**
 * @file ard_network_layer_three_message.h
 * @brief Classes modeling messages at different layers of the communication
 * stack
 *
 */

#ifndef ARD_NET_L3_MSG_H
#define ARD_NET_L3_MSG_H

#ifdef ARDUINO_AVR_NANO
#include <stdint.h> // for uint8_t
#else
#include <cstdint> // for uint8_t
#endif

#include "ard_mempool.h"
#include "ard_network_address.h"
#include "ard_pkt_buffer.h"

using PktBufPtr = ArdUniquePtrMemPool<ArdPktBuffer>;
using PktMemPool = ArdMemPool<ArdPktBuffer>;

/********************************************************************
 *                    Layer Three Messages                            *
 ********************************************************************/


/**
 * @Brief L3Message models a Layer Message
 *
 * To build a message:
 *   - Declare a new L3Message object.
 *   - Fill the member fields (m_dest_add, m_src_add).
 *     You can also use the constructor that takes as input parameters the
 *     values of each field).
 *   - Call serialize with the payload (using ard_move).
 *   - This returns a new memory buffer with the L3 header + the payload.
 *
 * When receiving a message:
 *   - Declare a new L3Message object.
 *   - Call deSerialize with message to decode.
 *   - This returns the payload (to be passed to upper layer)
 *     and fills the member fields (m_dest_add, m_src_add
 *     based on the data in the message received.
 *
 */
class L3Message {
public:
  /**
   * @brief Constructs a new L3Message object with default values in each field.
   *
   * This can be used when constructing an L3Message object that will be
   * used to deserialize a packet buffer.
   *
   */
  L3Message();

  /**
   * @brief Constructs an L3Message with given values for each field.
   *
   * This constructor can be used whenever all the fields are known at 
   * construction time (e.g., when sending a message).
   *
   * @param a_src_addr The source address.
   * @param a_dst_addr The destination address.
   * @param a_total_len The total length of the packet (header + payload)
   */
  L3Message(L3Addr a_src_addr, L3Addr a_dst_addr, uint8_t a_total_len);

  /**
   * @brief Serialize a packet.
   *
   * Once every member field has been filled, call this method with
   * the payload in parameter using ard_move
   * (eg. PktBufPtr p3 = l3.serialize(ard_move(p1));).
   *
   * @param a_sdu The payload from upper layer.
   * @param a_mem_pool A reference to the memory pool to use to 
   * allocate the packet buffer for the layer three frame.
   * @return The serialized packet to be passed to the lower layer.
   */
  ArdUniquePtrMemPool<ArdPktBuffer> serialize(PktBufPtr a_sdu,
                                              PktMemPool *a_mem_pool);

  /**
   * @brief DeSerialize a packet.
   *
   * Call this method with the received payload in parameter
   * using ard_move
   * (eg. PktBufPtr p_app = l3_back.deSerialize(ard_move(p3));).
   * This will extract header fields values in corresponding
   * object members (m_dest_add, m_src_add)
   * and return back the payload to be passed to the upper layer.
   *
   * @param a_pdu The payload from lower layer.
   * @param a_mem_pool A reference to the memory pool to use to 
   * allocate the packet buffer for the payload.
   * @return The packet payload to be passed to upper layer.
   */
  ArdUniquePtrMemPool<ArdPktBuffer> deSerialize(PktBufPtr a_pdu,
                                                PktMemPool *a_mem_pool);
  /**
   * @brief Parse an L3Message packet without transfering buffer
   * property (i.e. NOT having to use ard_move()).
   *
   * Call this method with the received payload in parameter
   * (eg. analyze(pdu->data, pdu->curr_size); ).
   * This will fill extract header fields values in corresponding
   * object members (m_dst_add, m_src_add).
   *
   * @param a_p_data The pointer to the buffer to parse (pdu->data).
   * @param a_length The length of the buffer to parse (pdu->curr_size).
   * @return True is parsing was successful False otherwise.
   */
  bool analyze(uint8_t *a_p_data, size_t a_length);

  L3Addr m_dst_add;      /// Destination Address
  L3Addr m_src_add;      /// Source Address
  uint8_t m_total_len;   /// The total length of the packet in bytes
  PktBufPtr m_pkt_buff;  /// Pointer to the packet buffer holding the result of the serialization
  constexpr static uint8_t l3_header_size = 5;
};

#endif // end include guard ARD_NET_L3_MSG_H
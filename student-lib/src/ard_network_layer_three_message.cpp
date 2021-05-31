
#include "ard_network_layer_three_message.h"
#include "ard_memcpy.h"

/********************************************************************
 *                    Layer Three Messages                            *
 ********************************************************************/
L3Message::L3Message()
    : m_dst_add(0), m_src_add(0) {}

L3Message::L3Message(L3Addr a_src_addr, L3Addr a_dst_addr,
                     uint8_t a_total_len)
    : m_dst_add(a_dst_addr), m_src_add(a_src_addr), m_total_len(a_total_len), 
    m_pkt_buff(nullptr) {}

ArdUniquePtrMemPool<ArdPktBuffer> L3Message::serialize(PktBufPtr a_sdu,
                                                       PktMemPool *a_mem_pool){
}

ArdUniquePtrMemPool<ArdPktBuffer> L3Message::deSerialize(PktBufPtr pdu,
                                                         PktMemPool *a_mem_pool){
}

bool L3Message::analyze(uint8_t *a_p_data, size_t a_length){
}


#ifndef ARD_PEER_CONTEXT_H
#define ARD_PEER_CONTEXT_H

#ifdef ARDUINO_AVR_NANO
#include <Arduino.h>
#include <stdint.h>
#else
#include <cstdint> // for uint8_t
#endif

#include "ard_error.h"
#include "ard_log.h"

/** \addtogroup most_important 
 *  @{
 */

using nodeAdd = uint8_t;


/**
 * @brief This class holds the Layer context relative to a given Node.
 *
 * The node is identified by its unit8_t address and stored in the 
 * m_address member field. 
 * 
 * Note: The companion class PeerContextPool is also provided to ease
 * PeerContext objects management. 
 *
 */
class PeerContext{
public:
  static constexpr nodeAdd invalid_add = 0xFF;

  /**
   * @brief Default constructor.
   * 
   * Initialise a PeerContext object with and address set at PeerContext::invalid_add. 
   */
  PeerContext()
    : m_address(invalid_add),
      m_send_seq_num(0),
      m_last_seq_num_processed((uint8_t)-1),
      m_last_seq_num_sent(0)
    {}

  /**
   * @brief Constructor.
   * 
   * Initialise a PeerContext object with and address set at a_add. 
   */
  PeerContext(nodeAdd a_add)
    : PeerContext()
     {
       m_address = a_add;
     }


public:
  nodeAdd m_address; /// The node address
  uint8_t m_send_seq_num;
  uint8_t m_last_seq_num_sent;
  uint8_t m_last_seq_num_processed;
// Note: Add any other necessary field here
//       Don't forget to initialise these field in the constructor
//       You can also rename the m_seq_num field is needed...
};




constexpr uint8_t MAX_NB_PEER = 8;


/**
 * @brief This holds MAX_NB_PEER PeerContext objects.
 *
 * PeerContext objects are indexed by their node Address. 
 *
 */
class PeerContextPool{
public:
  /**
   * @brief Default Constructor.
   * 
   * Initialise a the pool with MAX_NB_PEER PeerContext objects. 
   */
  PeerContextPool();

protected:
  PeerContext m_pool[MAX_NB_PEER]; /// THE context pool

public:
  /**
   * @brief Retrieve the PeerContext corresponding to the node with 
   * address a_address.
   *
   * Before calling retrieveContextByAdd(), it is MANDATORY to verify 
   * that the address has already been registered in the poll by 
   * calling isAddInPool().
   * 
   * @param a_address Address of the node to lookup.
   */
  PeerContext *retrieveContextByAdd(nodeAdd a_address);

  /**
   * @brief Tests if an entry for node with address a_address is 
   * already present in the pool.
   * 
   * @param a_address Address of the node to lookup.
   * @retrun true if node is present; false otherwise.
   */
  bool isAddInPool(nodeAdd a_address);

  /**
   * @brief Returns an unused PeerContext object for a new node with 
   * address a_address.
   *
   * Before calling retrieveContextByAdd(), it is MANDATORY to verify 
   * that the address was not already registered in the poll by 
   * calling isAddInPool().
   * 
   * @param a_address Address of the node to lookup.
   */
  PeerContext *getNewContextForAdd(nodeAdd a_address);

  /**
   * @brief Tests if the pool is full.
   * 
   * @retrun true if pool if full; false otherwise.
   */
  bool isFull();
};

/** @}*/

#endif // ARD_PEER_CONTEXT_H

#include "ard_peer_context.h"
PeerContextPool::PeerContextPool(){
  for (uint8_t i=0; i<MAX_NB_PEER; i++){
      m_pool[i].m_address = PeerContext::invalid_add;
  }
}

PeerContext* PeerContextPool::retrieveContextByAdd(nodeAdd a_address){
  for (uint8_t i=0; i<MAX_NB_PEER; i++){
      if (m_pool[i].m_address == a_address){
          return &(m_pool[i]);
      }
  }
  ard_error(ARD_F("Requesting context of an unknown node in PeerContextPool\n"));
  return NULL;  
}


bool PeerContextPool::isAddInPool(nodeAdd a_address){
  for (uint8_t i=0; i<MAX_NB_PEER; i++){
      if (m_pool[i].m_address == a_address){
          return true;
      }
  }
  return false;
}

 PeerContext* PeerContextPool::getNewContextForAdd(nodeAdd a_address){
  // Sanity check
  if (isAddInPool(a_address)){
    ard_error(ARD_F("Requesting to create a context of an already known node in PeerContextPool\n"));
    return retrieveContextByAdd(a_address);
  }
  if (isFull()){
    ard_error(ARD_F("Trying to add a new context in a full PeerContextPool\n"));
    return NULL;  
  }

  // Find an unused context
  uint8_t i;
  for (i=0; i<MAX_NB_PEER; i++){
      if (m_pool[i].m_address == PeerContext::invalid_add){
          break;
      }
  } // Normally sanity checks prevent from reaching MAX_NB_PEER
  if (i>=MAX_NB_PEER){
    ard_error(ARD_F("PeerContextPool full but this should not happen!\n"));
    return NULL;  
  }

  // Register this context for requested node
  m_pool[i].m_address = a_address;
  return &(m_pool[i]);
}

// bool PeerContextPool::addItemInPool(PeerContext a_newContext){
//   if (isFull()) return false;
//   if (isAddInPool(a_newContext.m_address)){
//       error_pr(ARD_F("Trying to add an already existing node in PeerContextPool\n"));
//       return false;
//   }
//   // find an empty slot
//   for (uint8_t i; i<MAX_NB_PEER; i++){
//       if (m_pool[i].m_address == PeerContext::invalid_add){
//           m_pool[i] = a_newContext;
//           return true;
//       }
//   }
//   error_pr(ARD_F("Noo room left in PeerContextPool\n"));
//   return false; // should not append is pool is not full...
// }


bool PeerContextPool::isFull(){
  for (uint8_t i=0; i<MAX_NB_PEER; i++){
      if (m_pool[i].m_address == PeerContext::invalid_add)
          return false;
  }
  return true;
}

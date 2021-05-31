#include <Arduino.h>
#include "ard_event_manager.h"
#include "ard_mempool.h"
#include "ard_sys_interface.h"
#include "ard_network_address.h"
#include "ard_network_layer_one.h"
#include "ard_network_layer_two.h"

#include "ard_network_eth.h"
#include "ard_network_layer_three_routing.h"


// Framework initialisation
ArdMemPool<ArdPktBuffer> g_mem_pool;
ArdSysInterfaceImpl g_sys_int;
ArdEventManager g_event_manager(&g_sys_int);

// Communication Stack
// Set your group number (minus one) here (eg. 5 for group #6)
uint8_t g_group_number  = YOUR_GROUP_NUMBER_MINUS_ONE; // Between 0 and 0x0E
L3Addr g_l3_addr_left = GetL3Address(APP_DEV_GATEWAY, g_group_number);
L3Addr g_l3_addr_right = GetL3Address(APP_DEV_GATEWAY, ArdNetConstants::occ_network_number);
L2Addr g_l2_addr_left  = GetL2Address(APP_DEV_GATEWAY, g_group_number);

// Layer 2:
SendInterfaceImplementation<L2Addr, L1Addr> l2_left_send_policy(g_l2_addr_left);
ArdNetworkLayerTwoAck g_l2_left(&g_mem_pool, &g_sys_int, &g_event_manager, 
                           &l2_left_send_policy, g_l2_addr_left);

// Layer 1: RFM12
ArdNetworkLayerOne g_l1(&g_mem_pool, &g_sys_int, &g_event_manager, L1Addr());
ARDRF12EventHandler g_rf12_event_handler(&g_l1, &g_sys_int);

// Layer 1&2: Ethernet
ArdNetworkEthernet g_eth(&g_mem_pool, &g_sys_int, &g_event_manager, L1Addr());


SendInterfaceImplementation<L3Addr, L2Addr> l3_left_send_policy(g_l3_addr_left);
SendInterfaceImplementation<L3Addr, L1Addr> l3_right_send_policy(g_l3_addr_right);
ArdNetworkRoutingLayerThree g_routing(&g_mem_pool, &g_sys_int, &g_event_manager,
                            &l3_left_send_policy, &l3_right_send_policy,
                            g_l3_addr_left, g_l3_addr_right);

void setup()
{
  // HW and Framework Intialisation
  Serial.begin(9600);
  rf12_initialize(1, RF12_868MHZ, 0xD4, 1600); // Radio module initialisation
  g_event_manager.registerEventHandler(&g_rf12_event_handler);

  // Communication Stack
  l3_right_send_policy.setLowerLayer(&g_eth);
  l3_left_send_policy.setLowerLayer(&g_l2_left);
  l2_left_send_policy.setLowerLayer(&g_l1);

  g_l1.setUpperLayer(&g_l2_left);
  g_l2_left.setUpperLayer(&g_routing);
  g_eth.setUpperLayer(&g_routing);

  // Applicatiton initialization
  g_eth.init();
}

void loop()
{
  g_event_manager.loopIteration();
}
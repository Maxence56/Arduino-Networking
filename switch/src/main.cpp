#include <Arduino.h>
#include "ard_event_manager.h"
#include "ard_mempool.h"
#include "ard_sys_interface.h"
#include "ard_network_address.h"
#include "ard_network_layer_one.h"
#include "ard_network_layer_two.h"
#include "ard_app_switch.h"


// Set your group number (minus one) here (eg. 5 for group #6)
uint8_t g_group_number  = YOUR_GROUP_NUMBER_MINUS_ONE; // Between 0 and 0x0E


// Framework initialisation
ArdMemPool<ArdPktBuffer> g_mem_pool;
ArdSysInterfaceImpl g_sys_int;
ArdEventManager g_event_manager(&g_sys_int);

// Compute Addresses
AppAddr g_app_addr = GetAppAddress(APP_DEV_SWITCH, g_group_number);
L2Addr  g_l2_addr  = mapAddr<AppAddr, L2Addr>(g_app_addr, g_app_addr);

// Layer 2:
SendInterfaceImplementation<L2Addr, L1Addr> l2_send_policy(g_l2_addr);
// Note: To activate the new version of layer2, swap the comments on the following lines
ArdNetworkLayerTwoThreeTxs g_l2(&g_mem_pool, &g_sys_int, &g_event_manager,
                                &l2_send_policy, g_l2_addr);
// ArdNetworkLayerTwoAck g_l2(&g_mem_pool, &g_sys_int, &g_event_manager, 
//                            &l2_send_policy, g_l2_addr);

// Layer 1: RFM12
ArdNetworkLayerOne g_l1(&g_mem_pool, &g_sys_int, &g_event_manager, L1Addr());
ARDRF12EventHandler g_rf12_event_handler(&g_l1, &g_sys_int);

// Switch Application
SendInterfaceImplementation<AppAddr, L2Addr> app_send_policy(g_app_addr);
AppSwitch g_app_layer(&g_mem_pool, &g_sys_int, &g_event_manager,
                      &app_send_policy, g_app_addr);


void setup()
{
  // HW and Framework Intialisation
  Serial.begin(9600);
  rf12_initialize(1, RF12_868MHZ, 0xD4, 1600); // Radio module initialisation
  g_event_manager.registerEventHandler(&g_rf12_event_handler);

  // Communication Stack
  app_send_policy.setLowerLayer(&g_l2);
  l2_send_policy.setLowerLayer(&g_l1);

  g_l1.setUpperLayer(&g_l2);
  g_l2.setUpperLayer(&g_app_layer);

  // Applicatiton initialization
  g_app_layer.m_notify_general_controller = false;
  g_app_layer.init();
}


void loop()
{
  g_event_manager.loopIteration();
}

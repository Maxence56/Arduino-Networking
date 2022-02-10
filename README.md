# Arduino-Networking

Documentation 👉🏻👉🏻 http://formations.imt-atlantique.fr/sar-res/doc/

## The project is split into three parts:

The goal of the first phase is to implement the reliability mechanism of the Layer 2 protocol.

In the second phase, you will generalize your reliability implementation to let one node communicate with several other nodes in parallel

In the third phase, you will develop the networking layer (layer 3) to manage the interconnection of several sub-networks.

### Phase 1: Reliability

Radio transmission are unreliable by nature. Implementing a reliability mechanism is one of the key goals of a "data link" (or "layer 2") protocol.

Several strategies can be used to increase the reliability of transmissions. The code that has been provided to you implements a very basic solution consisting in simply repeating each message several times (three times). While this solution does work correctly as long as not all three copies are lost, it does not work in all cases.

A better (and widely used) solution is to implement an acknowledgment based protocol. In this solution, when a node receives a message, it sends back a confirmation message (aka Acknowledgment) to the sender. If the sender doesn't receive the corresponding acknowledgment within a given period of time, it restarts the sending procedure (i.e., i sends the same message again and waits for the acknowledgment). This procedure is repeated until the acknowledgment is received (or until the maximum retransmission count is reached).

The goal of the phase 1 of the project is to implement a version of this mechanism known as "Send and Wait" that limits to at most one the number of unacknowledged messages at any given time. In other words, even when the sender has more than one message to send, it must send the first one and then wait for the corresponding acknowledgment before it can send the following message. The detailed specification of this protocol can be found in the Reliability section of the Layer 2 protocol specification

Note: For this phase, you will work with both the testing environment (debugger) and the actual platform. You can find some hints on how to handle several projects and several hardware targets in VS Code in the development environment page.

This pdf file shows all the objects and methods involved in sending (and receiving) a message and the associated acknowledgment. To better view the animation in the pdf file, download to your computer and view it with an external viewer, one page at a time. This way, you can use the arrow keys to move between the different steps.

Whenever a "layer" calls a method of another there is a arrow from the caller to the callee. The color of the arrow is the same color as the caller, while the color of method is the color of the owner of that method. For example, at the very beginning, when the Application (or layer three) calls sendRequest of layer two, the arrow is green because the caller is the application/layer three, while sendRequest is blue because the application/layer three calls the sendRequest method of layer two.

When the same data is present on two different nodes (sender and receiver) the figure uses a ' after the name of the variable to represent this. For example, l3_data represents the data sent by the application/layer three on the sender node; while l3_data' represents the same values but this time stored on the receiver.

Command Line Interface (CLI)
All three applications (Controller, Train and Switch) come with a user interface running on top of the serial port. The documentation of this Command Line Interface is available here.

How to Start ?
Your role is to develop the ArdNetworkLayerTwoAck class in the ard_network_layer_two.cpp and ard_network_layer_two.h files of the student-lib/src/ directory. You can look at the ArdNetworkLayerTwoThreeTxs class for inspiration. You can find it in the corresponding files.

Basically, you have to:

On the receiver:
Send an acknowledgement message for each received frame (except broadcast).
Filter possible duplicate received messages.
On the Sender:
Trigger a timer when a message is sent (except for acknowledgements).
Cancel this timer if the appropriate acknowledgement is received before timer expiration.
Resend the same message when the timer expires (or stop transmissions if the maximum number of repetition is reached).
Call the dataHandlingDone() method of the upper layer when needed.
Be warned that the same node can be a sender and a receiver, therefore it is more appropriate to talk about the "receiver code path" and "sender code path".
Don't forget to switch the version of the Layer 2 class (from ArdNetworkLayerTwoThreeTxs to ArdNetworkLayerTwoAck) in the setup() function of the entry point projects (controller, train and switch).
Tests for phase 1
As mentioned in the Development Environment section, you can use the automated tests written by the teaching staff to debug your implementation. You MUST pass at least the first tests in order to validate your implementation.

We use the Google Test and Google Mock libraries for the tests. These libraries organize tests in Test Suites, which are a collection of multiple tests (the curious reader can find more details here).

The current version of the tests, defines two test suites (NetworkLayerTwoTest and NetworkLayerTwoMultiNodeTest) with four and tow tests respectively. Each test instantiates your layer two implementation (ArdNetworklayerTwoAck) and it connects it to a "fake" application layer and to a fake layer one.

The first two tests, of the NetworkLayerTwoTest suite, verify that you have implemented correctly the behavior of the sender. They are:

SimpleSendRequest: this test calls the sendRequest method of your code (ArdNetworklayerTwoAck) with a payload and a destination address. It verifies that you call sendRequest method of layer one and that that packet buffer pointer that you pass to this method contains the correct layer two header followed by the payload. This is the simplest test in this test suite. You should start to pass this one first.
SendRequestOneTimeout: like the previous test, it calls the sendRequest method of your code (ArdNetworklayerTwoAck) with a payload and a destination address. Then it "simulates" the passage of time so that, if your implementation does use a timer, the timer will expire. It then verifies that you have called twice sendRequest method of layer one and that the packet buffer pointer that you pass to this method contains the correct layer two header followed by the payload.
The following two tests, verify that you have implemented correctly the behavior of the receiver. They are:

onDataReceived: this test calls the onDataReceived method of your code with a packet buffer pointer pointing to a buffer containing a valid layer two header followed by a payload. It verifies that:

you call onDataReceived of the "upper layer" and
you passed a packet buffer that contains only the payload and
the address parameter contains the correct address (i.e., the source address in the layer two header).
onDataReceivedPlusACK: like the previous test, it calls the onDataReceived method of your code with a packet buffer pointer pointing to a buffer containing a valid layer two header followed by a payload. Like the previous one, it verifies that you call onDataReceived of the upper layer with the correct values but then it also verifies that you have called sendRequest of layer one with a packet buffer containing the acknowledgment of the packet that you have just received.

Next tests: When finished with phase 1, you can proceed with phase 2 for which another set of tests is provided.

Warning
The tests can contain bugs! (Even teachers write codes with bugs!) Do not hesitate to post a message on the forum if you think that your code does the right thing but it does not pass the test.

### Phase 2: Communicating with Multiple Nodes
#### Goal
As you may have noticed, in the LayerTwoThreeTxs example, the code works only if the node communicates with a single client. Indeed, in the case where the node communicates with several other nodes, some context elements such as current sequence number may differ from one connection to another.

The goal of this second phase of the project is to make your layer two implementation able to manage this case.

Code Example
To structure the code, it a is good idea to declare classes to store the context relative to each connected node.

The two following files (ard_peer_context.h and ard_peer_context.cpp) have been pushed by the teachers on your repository and can be used as a starting point to implement this context management. They define two classes:

PeerContext: An object to store the context of a given node.
PeerContextPool: An object to manage a bunch of PeerContext objects.
You just have to:

Pull the last update of your git repository to get the ard_peer_context.h and ard_peer_context.cpp files in your student-lib directory.
In your ard_network_layer_two.h:
Add a PeerContextPool private member to your Layer Two management class.
Comment out the declaration of the members previously used to store the connection specific context(uint8_t m_send_seq_num;, uint8_t m_las_seq_num_rcv; and uint8_t m_last_seq_num_sent;)
Enrich the PeerContext with necessary information (i.e. the equivalent of the variables that you commented on the previous step).
Use these classes in your Layer Two implementation to store and retrieve the context of each connected node. For instance:
Ask the pool for a new PeerContext instance when communicating with a new node
or ask the pool to retrieve an already existing PeerContext for an already known node
Read or update this PeerContext instance with appropriate information
Tests for phase 2
The two tests of the NetworkLayerTwoMultiNodeTest suite verify that your implementation can correctly handle communications with more than one node. They are:

SendRequestTWoNodes this test calls the sendRequest method of your code (ArdNetworklayerTwoAck) three times: the first and second call ask to send a packet to the same node (call it A), the last call asks to send a packet to another node (call it B). The test calls the onDataReceived method of your code with the acknowledgments for the first two packets (those sent to A). It verifies that you call sendRequest of layer 1 three times with the correct packets.

OnDataReceivedTWoNodes this test calls the onDataReceived method of your code three times: with a (different) packet from node A the first two times and with a packet from node B the third time. It verifies that your code calls the onDataReceived of the upper layer with the payload from each packet and with the correct sending address. It also verifies that you send the acknowledgment of each packet to the corresponding sender.

Next tests: When finished with phase 3, you can proceed with phase 3 for which another set of tests is provided.

### Phase 3: Networking Protocol
#### Goal
In this part, your goal will be to develop a Layer 3 protocol to interconnect our embedded applications (train, controller and switch) with a Operation Control Center (OCC) on ground. The OCC is linked to our network through an Ethernet connection (another kind of Layer 2) and should be able to manage several independent train control systems (like ours). These train control systems can be provided by different suppliers, each having its own (and incompatible) layer 2 implementation.

In this context, you will have to (see the figure below):

Implement a common Layer 3 protocol that will be inserted between the application and the Layer 2 of the communication stack. One of the main features of this Layer 3 protocol will be to provide a unified addressing scheme for all networks. Thanks to that, any node will be able to communicate with any other node on any network, even if they don't use the same L2 protocol. Considering the analogy with the Internet, this is one of the functions of the famous IP protocols (IPv4 or IPv6), which are indeed located at the Layer 3 of the TCP/IP protocol stack.
Implement a router which will have two interfaces: one in the radio network (and running our Layer 2 radio protocol) and the other connected to the Ethernet network and implementing the Layer 2 protocol of the OCC. Your router will have to forward the packets received on each interface toward the other when needed and making appropriate Layer 2 substitution. Once again, Routers are commonly used on the Internet to interconnect different networks. You can think to your "Internet Box" that connects your local home wifi network to the network of your Internet Service Provider. Routing is also a typical function of the Layer 3 in the TCP/IP protocol stack.


#### What to do for Phase 3 

For this phase, you will start by implementing the Layer 3 protocol and then implement the routing logic of the router.

Implement the layer 3 protocol

Based on the Layer 3 protocol specification, implement the L3Message and ArdNetworkLayerThree classes. The header files (.h) are already provided in your student-lib directory. So you just have to add your code to the .cpp files.

Hints:

Start by implementing L3Message first.
You are strongly advised to inspire from the L2Message and ArdNetworkLayerTwo classes code.
Note that the L3 protocol is simpler than the L2 one! Basically all that you need to do is to add the L3 header to the payload when sending and removing it (plus some additional checks) upon reception.
To run this new L3/L3/Radio protocol stack on the Arduino boards, you need to use the modified version of the main.cpp files provided here.
Implementing the router

For this step, you will implement the ArdNetworkRoutingLayerThree class. For that, the ard_network_layer_three_message.h and ard_network_layer_three_message.cpp have been pushed to your student-lib directory. They contain the full header file (.h) and the skeleton of the .cpp file.

Implement the routing logic in the onDataReceived() method. This task is quite simple. Just think about the criteria to forward or not a packet from one side to the other side.

Note: in the context, we call left interface the L2 protocol instance linked to the radio side and right interface the one liked to the Ethernet wired network of the occ.

Tests for phase 3
There are four tests for the Layer 3. The first two test verify that your implementation can correctly add and remove the Layer 3 header. The last two validate your implementation of the routing logic.

L3 SendRequest : this test calls the sendRequest method of your Layer 3 (ArdNetworklayerThree) and verifies that you call the sendRequest method of the lower layer with a packet buffer pointer that points to a buffer containing the correct packet (i.e., layer 3 header plus the payload).
L3 OnDataReceived: This test calls the onDataReceived method of your Layer 3 (ArdNetworkLayerThree) and verifies that you call the onDataReceived method of the upper layer with the payload that was contained in the packet.
L3 RoutingToLeft: This test calls the onDataReceive method of your Layer 3 routing implementation (ArdNetworkRoutingLayerThree) with a packet that must be routed to the left interface and it verifies that you call the sendRequest method of the lower layer connected to the left interface.
L3 RoutingToRight: This test calls the onDataReceive method of your Layer 3 routing implementation (ArdNetworkRoutingLayerThree) with a packet that must be routed to the right interface and it verifies that you call the sendRequest method of the lower layer connected to the right interface.

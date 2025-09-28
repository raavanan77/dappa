#!/bin/bash
# create_ospf_topology.sh

# Create network namespaces (virtual routers)
sudo ip netns add router1
sudo ip netns add router2  
sudo ip netns add router3

# Create virtual ethernet pairs (virtual cables)
sudo ip link add veth1-2a type veth peer name veth1-2b  # Router1 <-> Router2
sudo ip link add veth2-3a type veth peer name veth2-3b  # Router2 <-> Router3
sudo ip link add veth1-3a type veth peer name veth1-3b  # Router1 <-> Router3

# Assign interfaces to namespaces
sudo ip link set veth1-2a netns router1
sudo ip link set veth1-2b netns router2
sudo ip link set veth2-3a netns router2  
sudo ip link set veth2-3b netns router3
sudo ip link set veth1-3a netns router1
sudo ip link set veth1-3b netns router3

# Configure IP addresses
# Router 1
sudo ip netns exec router1 ip addr add 10.1.2.1/24 dev veth1-2a  # Link to R2
sudo ip netns exec router1 ip addr add 10.1.3.1/24 dev veth1-3a  # Link to R3
sudo ip netns exec router1 ip link set veth1-2a up
sudo ip netns exec router1 ip link set veth1-3a up
sudo ip netns exec router1 ip link set lo up

# Router 2  
sudo ip netns exec router2 ip addr add 10.1.2.2/24 dev veth1-2b  # Link to R1
sudo ip netns exec router2 ip addr add 10.2.3.2/24 dev veth2-3a  # Link to R3
sudo ip netns exec router2 ip link set veth1-2b up
sudo ip netns exec router2 ip link set veth2-3a up
sudo ip netns exec router2 ip link set lo up

# Router 3
sudo ip netns exec router3 ip addr add 10.1.3.3/24 dev veth1-3b  # Link to R1
sudo ip netns exec router3 ip addr add 10.2.3.3/24 dev veth2-3b  # Link to R2  
sudo ip netns exec router3 ip link set veth1-3b up
sudo ip netns exec router3 ip link set veth2-3b up
sudo ip netns exec router3 ip link set lo up
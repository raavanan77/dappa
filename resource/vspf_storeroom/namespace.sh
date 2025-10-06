# Compile your OSPF daemon
make vspf

# Copy to a shared location
sudo cp ~/dappa/bulid/vspf.o /usr/local/bin/

# Run in each router namespace
sudo ip netns exec router1 /usr/local/bin/vspf.o --router-id 1.1.1.1 &
sudo ip netns exec router2 /usr/local/bin/vspf.o --router-id 2.2.2.2 &
sudo ip netns exec router3 /usr/local/bin/vspf.o --router-id 3.3.3.3 &
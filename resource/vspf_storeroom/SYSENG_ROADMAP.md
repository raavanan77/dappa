# 3-Week Linux Systems Programming Roadmap

> **Goal**: Master Linux systems programming from userspace applications to kernel modification
> **Time Commitment**: 10 hours/day (6 hours deep focus + 4 hours practice)
> **Target**: Build OSPF routing protocol implementation

## Week 1: Core Systems Fundamentals

### Day 1-2: Memory Management Deep Dive

#### Concepts to Study
- [ ] Virtual memory architecture and page tables
- [ ] Heap vs stack vs data segments memory layout
- [ ] Memory allocation strategies (malloc internals, slab allocator)
- [ ] Memory mapping (`mmap`, `munmap`, `MAP_SHARED` vs `MAP_PRIVATE`)
- [ ] Memory protection and segmentation faults
- [ ] NUMA (Non-Uniform Memory Access) basics

#### Things to Look After
- [ ] Buffer overflows and memory corruption detection
- [ ] Memory leaks and use-after-free bugs
- [ ] Alignment issues and structure padding
- [ ] Memory barriers and cache coherency
- [ ] Valgrind and AddressSanitizer usage

#### Hands-on Projects
```c
// Build a custom memory allocator
// Implement shared memory communication between processes
// Debug memory corruption with valgrind/gdb
// Create memory pool for packet allocation
```

#### Resources
- [ ] "Understanding the Linux Virtual Memory Manager" - Mel Gorman
- [ ] `/proc/meminfo` and `/proc/[pid]/maps` analysis
- [ ] `man 2 mmap`, `man 3 malloc`

---

### Day 3-4: Advanced Pointers & Data Structures

#### Concepts to Study
- [ ] Pointer arithmetic and void pointer manipulation
- [ ] Function pointers and callback mechanisms
- [ ] Complex data structures (AVL trees, hash tables, tries)
- [ ] Memory-efficient data structures
- [ ] Lock-free data structures basics (atomic operations)

#### Things to Look After
- [ ] Dangling pointers and null dereferences
- [ ] Pointer aliasing issues and strict aliasing rules
- [ ] Cache-friendly data layouts (struct packing)
- [ ] Memory fragmentation patterns
- [ ] Endianness handling for network protocols

#### Hands-on Projects
```c
// Implement hash table with collision handling
// Build callback-based event system
// Create lock-free queue for packet processing
// Design cache-efficient routing table structure
```

---

### Day 5-7: Inter-Process Communication (IPC)

#### Concepts to Study
- [ ] Pipes (anonymous and named/FIFOs)
- [ ] Message queues (POSIX and System V)
- [ ] Shared memory segments (`shm_open`, `shmget`)
- [ ] Unix domain sockets
- [ ] Signals and signal handling (`sigaction`, `signalfd`)
- [ ] Semaphores and mutexes for IPC synchronization

#### Things to Look After
- [ ] Race conditions in IPC operations
- [ ] Deadlock prevention strategies
- [ ] Signal safety and async-signal-safe functions
- [ ] Proper cleanup of IPC resources
- [ ] Performance characteristics of different IPC methods

#### Hands-on Projects
```c
// Build multi-process chat server using different IPC methods
// Implement producer-consumer with shared memory
// Create signal-based process coordination system
// Design IPC protocol for routing daemon communication
```

---

## Week 2: Advanced Systems Programming

### Day 8-9: Process & Thread Management

#### Concepts to Study
- [ ] Process creation (`fork`, `exec` family, `clone`)
- [ ] Process groups and sessions
- [ ] Thread creation and management (`pthreads`)
- [ ] Thread synchronization (mutexes, condition variables, barriers)
- [ ] Thread-local storage and per-CPU variables
- [ ] Process scheduling and priorities

#### Things to Look After
- [ ] Zombie processes and proper child reaping
- [ ] Thread safety and data races
- [ ] Priority inversion problems
- [ ] Resource leaks in multi-threaded applications
- [ ] Signal handling in multi-threaded programs

#### Hands-on Projects
```c
// Implement thread pool for packet processing
// Build multi-process server with proper cleanup
// Create worker thread pattern for OSPF LSA processing
// Implement process supervision and restart logic
```

---

### Day 10-11: Network Programming & Raw Sockets

#### Concepts to Study
- [ ] Socket programming (TCP/UDP deep dive)
- [ ] Raw sockets and custom protocol implementation
- [ ] Network packet structure and parsing
- [ ] Socket options and advanced features (`SO_REUSEPORT`, `SO_BINDTODEVICE`)
- [ ] Non-blocking I/O and event-driven programming
- [ ] Network byte order and endianness

#### Things to Look After
- [ ] Proper error handling in network code
- [ ] Buffer management for variable-length packets
- [ ] Network congestion and flow control
- [ ] Security considerations in raw socket programming
- [ ] Performance optimization for high packet rates

#### Hands-on Projects
```c
// Implement custom protocol over raw sockets
// Build packet sniffer and analyzer
// Create non-blocking multi-client server
// Design OSPF packet format and parsing
```

---

### Day 12-14: I/O Multiplexing & Event Systems

#### Concepts to Study
- [ ] `select`, `poll`, and `epoll` system calls
- [ ] Event-driven programming patterns
- [ ] Asynchronous I/O (`aio` family, `io_uring`)
- [ ] File descriptor management
- [ ] Timer management (`timerfd`, `timer_create`)

#### Things to Look After
- [ ] File descriptor leaks
- [ ] Spurious wakeups and edge-triggered vs level-triggered
- [ ] Scalability issues with different I/O multiplexing methods
- [ ] Timer accuracy and drift
- [ ] Signal handling in event loops

#### Hands-on Projects
```c
// Build epoll-based high-performance server
// Implement timer wheel for protocol timeouts
// Create event-driven state machine
// Design scalable connection manager
```

---

## Week 3: Network Protocols & Kernel Integration

### Day 15-17: Protocol State Machines & OSPF Implementation

#### Concepts to Study
- [ ] State machine design patterns
- [ ] OSPF protocol specification (Hello, LSA, SPF)
- [ ] Dijkstra's algorithm implementation
- [ ] Network topology representation
- [ ] Routing table management

#### Things to Look After
- [ ] State transition race conditions
- [ ] Protocol timer management
- [ ] Neighbor discovery and failure detection
- [ ] LSA flooding algorithm correctness
- [ ] Convergence time optimization

#### Hands-on Projects
```c
// Implement OSPF neighbor state machine
// Build link-state database with proper synchronization
// Create SPF calculation engine
// Design network topology discovery
```

---

### Day 18-19: Kernel Networking Interface

#### Concepts to Study
- [ ] Netlink sockets for kernel communication
- [ ] Routing table manipulation from userspace
- [ ] Network namespaces and containers
- [ ] Traffic control (tc) and Quality of Service
- [ ] Packet filtering and netfilter hooks

#### Things to Look After
- [ ] Kernel version compatibility
- [ ] Privilege requirements for kernel operations
- [ ] Race conditions in routing table updates
- [ ] Proper cleanup of kernel resources
- [ ] Error handling for kernel communication

#### Hands-on Projects
```c
// Implement routing table updates via netlink
// Create network namespace isolation
// Build custom packet filter
// Design kernel-userspace communication protocol
```

---

### Day 20-21: Performance & Production Readiness

#### Concepts to Study
- [ ] Profiling tools (`perf`, `gprof`, `callgrind`)
- [ ] Memory profiling (`valgrind`, `tcmalloc`)
- [ ] Network performance optimization
- [ ] Lock-free programming patterns
- [ ] NUMA-aware programming

#### Things to Look After
- [ ] CPU cache efficiency
- [ ] Memory allocation patterns
- [ ] Lock contention and scalability
- [ ] Network latency optimization
- [ ] Error recovery and fault tolerance

#### Hands-on Projects
```c
// Profile and optimize OSPF implementation
// Implement zero-copy packet processing
// Create lock-free routing table updates
// Build comprehensive error recovery system
```

---

## Essential Resources

### Books
- [ ] "Advanced Programming in the UNIX Environment" - Stevens & Rago
- [ ] "Linux System Programming" - Robert Love
- [ ] "Understanding the Linux Kernel" - Bovet & Cesati
- [ ] "TCP/IP Illustrated Volume 1" - Stevens
- [ ] "Linux Network Programming" - Kerrisk

### Documentation
- [ ] Linux manual pages (`man 2`, `man 3`, `man 7`)
- [ ] `/usr/include/linux/` header files
- [ ] RFC 2328 (OSPF Version 2)
- [ ] Linux kernel documentation (`Documentation/networking/`)

### Tools to Master
- [ ] **Debugging**: `gdb`, `strace`, `ltrace`, `valgrind`
- [ ] **Profiling**: `perf`, `oprofile`, `callgrind`
- [ ] **Network**: `tcpdump`, `wireshark`, `netstat`, `ss`
- [ ] **System**: `htop`, `iotop`, `lsof`, `proc filesystem`

---

## Daily Structure (10-hour schedule)

### Deep Focus Block (6 hours)
- **2 hours**: Reading and concept study
- **3 hours**: Hands-on coding and implementation  
- **1 hour**: Code review and debugging

### Practice Block (4 hours)
- **2 hours**: Laboratory exercises and experimentation
- **1 hour**: Documentation and note-taking
- **1 hour**: Community interaction (forums, IRC, code review)

---

## Success Metrics

### Week 1 Checkpoints
- [ ] Built custom memory allocator without leaks
- [ ] Implemented IPC communication between 5+ processes
- [ ] Debugged complex pointer arithmetic issues
- [ ] Created thread-safe data structures

### Week 2 Checkpoints  
- [ ] Built high-performance network server (1000+ connections)
- [ ] Implemented custom network protocol
- [ ] Created event-driven state machine
- [ ] Optimized I/O performance significantly

### Week 3 Checkpoints
- [ ] Working OSPF implementation across multiple VMs
- [ ] Kernel routing table integration
- [ ] Sub-second convergence time achieved
- [ ] Production-ready error handling and logging

---

## Final Project: OSPF Router Implementation

### Architecture Components
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Hello Process  │    │  LSA Database    │    │  SPF Calculator │
│                 │    │   (Shared Mem)   │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────────┐
                    │   Routing Engine    │
                    │  (Netlink Interface) │
                    └─────────────────────┘
```

### Key Features to Implement
- [ ] Multi-area OSPF support
- [ ] Dynamic neighbor discovery
- [ ] LSA flooding with loop prevention
- [ ] Dijkstra SPF calculation
- [ ] Routing table synchronization
- [ ] Graceful restart capability
- [ ] Performance monitoring and metrics

---

## Notes Section

### Common Pitfalls to Avoid
- Memory leaks in long-running daemons
- Race conditions in multi-threaded network code
- Improper signal handling causing zombie processes
- Network byte order issues in protocol headers
- Resource exhaustion under high load

### Debugging Strategies
- Always use version control with meaningful commits
- Implement comprehensive logging from day one
- Use static analysis tools (`cppcheck`, `clang-static-analyzer`)
- Test in VMs to avoid breaking host system
- Keep separate debugging and production builds

### Community Resources
- Linux Kernel Newbies: kernelnewbies.org
- NetworkEngineering subreddit for protocol discussions
- #networking on Libera.Chat IRC
- Linux kernel mailing lists for advanced questions

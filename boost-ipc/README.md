Inter‑Process Comms Base Folder (C++ / Boost.Interprocess / CMake)
Refer to build details for process to install/build.
This is a base structure for testing multi‑process communication systems built in C++ using Boost.Interprocess.
It simulates a simple control‑system architecture, allowing two independent executables to exchange messages 
using a reliable IPC mechanism.

The setup provides:
- multi‑process structure
- message‑queue based IPC
- synch and blocking
- reproducible CMake build system
- dependency management using vcpkg
- useful for windows or linux systems programming and base testing
                      BOOST MSG QUEUE (request, read, respond)
- process_a (prod) --> a_to_b queue --> process_b (cons)
- process_a (cons) <-- b_to_a queue <-- process_b (prod)
- 
- process_a sends data (req) --> process_b (read) and can then respond
- process_a reads (read/resp) <-- process_b sends back data (resp)

Simple skeleton: 
process_a sends commands and receives responses
process_b receives commands and sends responses
Communication uses Boost.Interprocess message queues

Benefits of Boost:
- cross‑platform message queues
- deterministic blocking behaviour
- fixed‑size messages suitable for real‑time systems
- no OS‑specific Win32 API calls
- predictable memory usage

Structure:
- WindowsProcessIPCpipes/
│   CMakeLists.txt
│   process_a.cpp
│   process_b.cpp
│
└── build/
    └── Debug/
        ├── process_a.exe
        └── process_b.exe

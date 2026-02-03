# control_system
+-----------------------------------------------------------+
|                       WPF APP                             |
|-----------------------------------------------------------|
| - UI (controls, status display)                           |
| - JSON serializer/deserializer                            |
| - TCP SOCKET CLIENT                                       |
|     * connects to C++ WPF-facing socket server            |
|     * sends commands as JSON payloads                     |
|     * receives status/telemetry as JSON payloads          |
+-----------------------------------------------------------+
                              ||
                              ||  TCP (JSON payloads)
                              \/
+-----------------------------------------------------------+
|                   C++ CONTROL SYSTEM                      |
|-----------------------------------------------------------|
| CORE LOGIC                                                |
|  - Control algorithms                                     |
|  - State machine                                          |
|  - I/O abstraction (inbits/outbits)                       |
|                                                           |
| WPF SOCKET SERVER                                         |
|  - Listens for WPF client connections                     |
|  - Parses incoming JSON commands                          |
|  - Updates internal control state                         |
|  - Sends JSON responses / status                          |
|                                                           |
| UNITY SOCKET CLIENT                                       |
|  - Connects to Unity TCP server                           |
|  - Sends OUTBIT STATUS as JSON                            |
|       { "outbits": [0,1,0,1,...] }                        |
|  - Receives INBIT STATUS as JSON                          |
|       { "inbits":  [1,0,1,0,...] }                        |
+-----------------------------------------------------------+
                              ||
                              ||  TCP (JSON payloads: inbit/outbit)
                              \/
+-----------------------------------------------------------+
|                    UNITY SIMULATION                       |
|-----------------------------------------------------------|
| - Simulation of plant / environment                       |
| - Visual 3D/2D scene                                      |
|                                                           |
| UNITY SOCKET SERVER                                       |
|  - Accepts connection from C++ Unity client               |
|  - Receives OUTBIT STATUS JSON                            |
|       { "outbits": [...] }                                |
|  - Updates conveyor state                                 |  
|  - Computes resulting INBIT STATUS JSON                   |
|       { "inbits":  [...] }                                |
|  - Sends INBIT STATUS JSON back to C++ client             |
+-----------------------------------------------------------+

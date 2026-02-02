# control_system

┌─────────────────┐         ┌─────────────────┐         ┌─────────────────┐
│   WPF UI        │         │  C++ Server     │         │  Unity          │
│  (VS Solution)  │────────>│  (VS Solution)  │────────>│ (Separate App)  │
│                 │  sends  │                 │broadcast│                 │
│  Port: Client   │  cmds   │  Port: 5000     │  cmds   │  Port: Client   │
└─────────────────┘         └─────────────────┘         └─────────────────┘
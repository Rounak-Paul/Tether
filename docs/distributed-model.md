# Tether — Distributed Node Model & Scheduler

This document describes the design for Tether's distributed execution model: node executables, registration, workload distribution, auto load-balancing, and failure handling. It assumes the Phase 1 bootstrap and runtime are written in C.

## Goals

- Allow users to run a `.t` script locally while transparently distributing work across available CPUs, GPUs, and remote nodes.
- Provide a lightweight node executable (`tether-node`) that can be deployed on devices (edge, cloud VMs, servers).
- Autonomously balance work across local threads, local devices, and remote nodes without user intervention, but provide explicit hints/controls when desired.
- Safe defaults with observable diagnostics and clear failure modes.

## Node executable (`tether-node`)

- Minimal C program exposing a network API to a central controller (the developer's local runtime or a cluster manager).
- Responsibilities:
  - Register with controller and advertise capabilities (CPU cores, GPU devices, memory, network bandwidth, tags).
  - Accept tasks (kernels, data transfers, function calls) and execute them in an isolated runtime.
  - Report heartbeats, resource usage, and task completion/failure.
  - Provide a secure RPC channel (mutual TLS recommended) for authentication and encryption.

## Controller & scheduler model

- The developer's local runtime acts as a controller/scheduler for the scope of the running `.t` script.
- Controller responsibilities:
  - Discover local device capabilities and remote nodes (via direct config, mDNS, or static list).
  - Maintain node state and metrics (current load, queued tasks, health).
  - Decide placement for each parallel region / kernel: local CPU threads, local GPU, remote node(s), or hybrid.
  - Split tasks into chunks for distribution (work-stealing model across nodes).

## Task model & code shipping

- Tasks are serializable units of work: kernel ID + typed arguments + data access descriptors.
- Code shipping: the runtime ships a compact device-friendly representation (AOT or bytecode) of the kernel to nodes; nodes validate signatures and run code.
- Data transfer: the controller schedules necessary data transfers. Large arrays can be pinned on nodes to avoid repeated copies.

## Auto load-balancing

- Heuristics used by the controller:
  - Estimate task cost from kernel complexity and input sizes.
  - Use device capacity (cores, flops, memory) to compute throughput estimates.
  - Prefer local CPU/GPU for small tasks to avoid transfer overhead.
  - Split large data-parallel tasks into chunks and distribute proportionally to node capacity.
  - Rebalance dynamically: nodes report progress; controller can reassign overdue chunks to other nodes (speculative execution).

## Fault tolerance & failure modes

- Heartbeats: nodes send heartbeats; missing heartbeats mark node as unhealthy.
- Checkpointing: for long-running tasks, support lightweight checkpointing or chunk-level idempotency so failed chunks can be retried.
- Partial results: controller must handle partial completion and assemble final outputs; tasks should be deterministic where possible.

## Security

- Mutual authentication between controller and nodes (TLS + JWT tokens or mTLS certs).
- Node sandboxing: run tasks in a restricted environment with limited OS privileges.

## APIs & CLI

- `tether-node --register --controller <addr> --capabilities json` — startup registration.
- Runtime API (controller ↔ node):
  - Register/Unregister
  - Heartbeat
  - SubmitTask
  - QueryStatus
  - FetchLogs

## Next steps (implementation plan)

1. Implement `tether-node` skeleton in C with registration and heartbeat (Phase 1).
2. Implement controller-side scheduler heuristics and a simple proof-of-concept: distribute a parallel_for across local threads and one remote node.
3. Expand to multiple nodes, add secure channels, and refine heuristics with profiling.

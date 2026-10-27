*This project has been created as part of the 42 curriculum by mel-bakh.*

# Codexion

## Description

Codexion models coders sharing USB dongles. This repository currently implements
one concurrent compile attempt per coder. Each successful compile reserves both
neighboring dongles together, waits for the configured compile duration, and then
releases both dongles. The remaining simulation phases and burnout monitor are not
implemented yet.

## Instructions

Build with `make`, then run:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All durations are milliseconds. `scheduler` is `fifo` or `edf`. For example:

```sh
./codexion 4 1000 20 10 10 1 5 fifo
```

The current implementation uses `time_to_compile` and `dongle_cooldown` during
the single compile attempt. It accepts the other required arguments but does not
run debug or refactor phases, repeat compiles, or detect burnout yet. With one
coder there is only one dongle, so no compile can start; that thread exits.

## Blocking cases handled

- Atomic two-dongle reservation prevents hold-and-wait deadlocks.
- Every request enters both dongle heaps under the arbiter mutex. A coder takes
  the pair only when it heads both heaps, both dongles are available, and both
  cooldowns have expired.
- FIFO and EDF use the existing heap ordering, with request time and coder ID as
  tie breakers. The shared order prevents a cycle of queue heads.
- Waiting coders sleep on a condition variable. A release or cancellation wakes
  them; a timed wait handles cooldown expiration.
- Cooperative stop removes pending requests from both heaps before threads exit.

Starvation prevention across repeated compile cycles and burnout detection are
not complete because those phases are not implemented yet.

## Thread synchronization mechanisms

The simulation arbiter mutex protects request queues, dongle state, and the stop
flag during reservation decisions. Each dongle also has a mutex; pair operations
lock them in ID order. A simulation condition variable wakes waiting coders after
release, cancellation, or stop. A separate log mutex prevents output lines from
interleaving. Threads are joined before mutexes, heaps, and dongles are freed.

## Resources

- POSIX manual pages: `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`,
  `pthread_cond_timedwait(3)`, and `clock_gettime(2)`.
- The project subject at `src/codexion.pdf`.
- AI assistance was used to design and implement the two-dongle reservation,
  write the tests and review, and check 42 Norm compliance. The behavior and
  remaining limitations should be reviewed with peers before evaluation.

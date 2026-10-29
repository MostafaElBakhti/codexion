*This project has been created as part of the 42 curriculum by mel-bakh.*

# Codexion

## Description

Codexion models coders sharing USB dongles. Each coder repeatedly acquires two
neighboring dongles, compiles, releases them, debugs, and refactors. A separate
monitor stops the simulation when a coder reaches a burnout deadline. The run
also stops when every coder has completed the required number of compiles.

## Instructions

Build with `make`, then run:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All durations are milliseconds. `scheduler` is `fifo` or `edf`. For example:

```sh
./codexion 4 1000 20 10 10 1 5 fifo
```

With one coder there is one dongle. The coder takes it, cannot compile, and
eventually burns out.

## Blocking cases handled

- Atomic two-dongle reservation prevents hold-and-wait deadlocks.
- Every request enters both dongle heaps under the arbiter mutex. A coder takes
  the pair only when it heads both heaps, both dongles are available, and both
  cooldowns have expired.
- FIFO uses request arrival order, including requests in the same millisecond.
  EDF uses the earliest burnout deadline. Equal EDF deadlines favor alternating
  seats, which lets independent coders compile in parallel.
- Waiting coders sleep on a condition variable. A release or cancellation wakes
  them; a timed wait handles cooldown expiration.
- Cooperative stop removes pending requests from both heaps before threads exit.

EDF completed repeated-cycle test runs with up to 100 coders. These finite tests
do not prove starvation freedom for every feasible configuration.

## Thread synchronization mechanisms

The simulation arbiter mutex protects request queues, stop, compile counts, and
compile start times. Each dongle also has a mutex; pair operations lock them in
ID order. The monitor waits on a condition variable until the next deadline and
wakes when compile starts change deadlines. Release, cancellation, and stop wake
waiting coders. State logging takes the log mutex while holding the arbiter
mutex, so no ordinary state line follows a burnout line. Threads are joined
before mutexes, heaps, and dongles are freed.

## Resources

- POSIX manual pages: `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`,
  `pthread_cond_timedwait(3)`, and `clock_gettime(2)`.
- The project subject at `src/codexion.pdf`.
- AI assistance was used to design and implement the two-dongle reservation,
  lifecycle and monitor, and to develop test scenarios. The behavior and
  remaining limitations should be reviewed with peers before evaluation.

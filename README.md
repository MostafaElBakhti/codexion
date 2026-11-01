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

- Atomic two-dongle reservation prevents a coder from holding one dongle while
  waiting for another, removing the hold-and-wait condition for deadlock. Pair
  mutexes are always locked in dongle ID order, preventing a circular lock wait.
- Every request enters both dongle heaps under the arbiter mutex. A coder takes
  the pair only when it heads both heaps, both dongles are available, and both
  cooldowns have expired.
- For the first compile, odd-numbered coders wait until all even-numbered
  coders have submitted their requests. The even seats use separate dongle
  pairs, preventing an initial chain of adjacent FIFO waiters from forcing
  everyone to compile in sequence. This startup gate uses the arbiter condition
  variable and is skipped on later cycles. Requests receive their arrival
  timestamp and sequence only after passing the gate; queued FIFO requests
  keep their original order.
- FIFO uses request arrival order, including requests in the same millisecond.
  EDF prioritizes the earliest burnout deadline. Equal EDF deadlines favor even
  seats before odd seats, then request arrival order. This can allow coders with
  separate dongle pairs to compile in parallel.
- Waiting coders sleep on a condition variable. A release or cancellation wakes
  them; a timed wait handles cooldown expiration.
- Cooperative stop removes pending requests from both heaps before threads exit.
- The monitor waits until the next burnout deadline and wakes when a compile
  start changes a deadline. It logs burnout and stops the simulation under the
  arbiter mutex. A separate log mutex keeps output lines intact, and the shared
  lock order prevents a coder state line from appearing after burnout.

EDF priority is intended to reduce starvation, but repeated-cycle test runs with
up to 100 coders do not prove that every feasible configuration avoids it.

## Thread synchronization mechanisms

The simulation arbiter `pthread_mutex_t` protects request queues, the startup
request count, stop, compile counts, and compile start times. Each dongle also
has a `pthread_mutex_t`; pair
operations lock them in ID order. For example, two coders sharing a dongle
cannot both reserve it: the arbiter serializes their requests, and the dongle
mutex protects its availability and cooldown state. The monitor uses the shared
`pthread_cond_t` with `pthread_cond_timedwait` to sleep until the next deadline;
a compile start broadcasts on that condition variable so it can recalculate the
deadline. Release, cancellation, and stop also broadcast to
waiting coders. Logging takes the log mutex while holding the arbiter mutex, so
the monitor's stop decision and state messages stay in order. Threads are joined
before mutexes, heaps, and dongles are freed.

## Resources

- POSIX manual pages: `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`,
  `pthread_cond_timedwait(3)`, and `clock_gettime(2)`.
- The project subject at `src/codexion.pdf`.
- AI assistance was used to design and implement the two-dongle reservation,
  lifecycle and monitor, and to develop test scenarios. The behavior and
  remaining limitations should be reviewed with peers before evaluation.

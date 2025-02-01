# To-do

- In the `listen` module,
  rename 'monitor' to 'receiver'
  to make clear that 'receiver' is an interface
  and not tied to the `monitor` module implementation.

- Replace GNU Make with *meson*.

- Separate program into application and library componenents.

- Make Python bindings.

- Un-unify the result types
  but make it easier to define new result types
  and create a common `adptc_os_result` type for system call results
  which can be composed into the other result types.

- Make `SIGINT` shutdown gracefully.

- Avoid `abort`ing in library code.
  Perhaps the application registers a panic handler
  which is invoked by `adptc_panic_internal`.
  The default panic handler, if one is not registered,
  could be `abort` though.

- Improve error handling in the `console` and `listen` modules.

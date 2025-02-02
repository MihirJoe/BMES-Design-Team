# To-do

- Finish Python bindings.
  In particular, finish the `_adapt_client` extension module
  and then create a higher-level `adapt_client` module in Python.

- Replace `adptc_listener_routine` with `adptc_listener_callback`
  that is called when incoming data is available.
  This is a better design, for multiple reasons.

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

# To-do

- Finish Python bindings.
  In particular, finish the `_adapt_client` extension module
  and then create a higher-level `adapt_client` module in Python.

- Un-unify the result types
  but make it easier to define new result types
  and create a common `adptc_os_result` type for system call results
  which can be composed into the other result types.

- Make `SIGINT` shutdown gracefully.

- Improve error handling in the `console` and `listen` modules.

# To-do

- Finish Python bindings.
  In particular, finish the `_adapt_client` extension module
  and then create a higher-level `adapt_client` module in Python.

- Record serial error information in `incoming`.
  Create functions to report this information.

- In the CPython extension module,
  don't bind to `adptc_listener_incoming` directly
  but provide an `IncomingResponses` iterator or such
  that invokes the response decoder and yields responses.

- Make `SIGINT` shutdown gracefully.

- Improve error handling in the `console` and `listener` modules.

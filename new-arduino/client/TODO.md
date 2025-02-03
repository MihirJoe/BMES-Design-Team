# To-do

- Finish Python bindings.
  In particular, finish the `_adapt_client` extension module
  and then create a higher-level `adapt_client` module in Python.

- Rename the `listener` module back to `listen`
  but keep the `adptc_listener` and `listener` types.

- Undo adding fill ratio information as part of the `listen` module itself,
  but provide an `adptc_listen_incoming_buf_size` object
  with static storage duration and constant value `LISTEN_BUF_SIZE`.
  This way, the extension module can determine the fill ratio, if it needs it,
  from the incoming data length and the buffer size.

- Consider renaming
  the `data_len` and `data_buf` fields of `incoming`
  to just `len` and `buf`.
  `incoming` means "incoming data," anyway.
  Also rename the corresponding functions.

- Record serial error information in `incoming`.
  Create functions to report this information.

- In the CPython extension module,
  don't bind to `adptc_listen_incoming` directly
  but provide an `IncomingResponses` iterator or such
  that invokes the response decoder and yields responses.

- Make `SIGINT` shutdown gracefully.

- Improve error handling in the `console` and `listen` modules.

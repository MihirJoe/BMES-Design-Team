# To-dos

The code is taking shape,
but the listener thread should have as its sole responsibility
monitoring the serial line
and leave response handling and unmarshalling to the interactive loop.
The listener thread should signal the interactive loop
when a new chunk of serial data is ready;
then, the listener thread is blocked
until the interactive loop signals back that it is ready for the next chunk.

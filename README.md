# TCP server and client to send texts
this project was made to learn sockets with C. it's a simple TCP server and client to send message, and get the message's length back.

## why is the code commented that much ?
i added the most comments possible to keep a trace of what i was doing, and also to know what headers to use and what do they specifically have.

## NEW: 06/10/2018
i heard using `fork(2)` is a real shit, and i should use `poll(2)` instead. so, basically, i learnt what i needed. it worked as intended so far, and i'm now focusing on making a basic IRC server and clients. maybe ncurses, maybe just plain terminal.

# mongoTest
Shared repo for testing new networking code with MongoDB server

# Building

`make`

# Running

Start the OMPI name resolution service and copy the outputted port string for the commands following.

```
ompi-server --no-daemonize -r -
```

Start the server. The OMPI server port should be in quotes.

```
mpirun -np 1 --ompi-server "<ompi_server_port>" server
```

Start the client

```
mpirun -np 1 --ompi-server "<ompi_server_port>" client
```


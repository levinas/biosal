biosal is a distributed BIOlogical Sequence Analysis Library.

This is a work in progress.

Looking for the Application Programming Interface (API) ? Look no further !

===> [biosal API](Documentation/API.md) <===

# Technologies

- Name: biosal
- Language: C99 (ISO/IEC 9899:1999)
- Model: actor model
- Message passing: MPI 2.2
- Threads: Pthreads (IEEE Std 1003.1c - 1995)

# Try it out

```bash
git clone https://github.com/GeneAssembly/biosal.git
cd biosal
make test # run tests
make run # run examples
```

see [examples](examples) and [tests](tests)

# Branches

Branch | Browse | HTTPS | SSH
--- | --- | --- | ---
 master | https://github.com/GeneAssembly/biosal/tree/master | https://github.com/GeneAssembly/biosal.git | git@github.com:GeneAssembly/biosal.git
 entropy (Fangfang's development branch) | https://github.com/levinas/biosal/tree/entropy | https://github.com/levinas/biosal.git | git@github.com:levinas/biosal.git
 energy (Seb's development branch) | https://github.com/sebhtml/biosal/tree/energy | https://github.com/sebhtml/biosal.git | git@github.com:sebhtml/biosal.git

see 'git log' for authors

# License

biosal is licensed under the [The BSD 2-Clause License](LICENSE.md)

# Tickets

- https://github.com/GeneAssembly/biosal/issues?state=open
- Website: https://github.com/GeneAssembly/biosal

# Actor model

The actor model has actors and messages, mostly.

When an actor receives a message, it can ([Agha 1986](http://dl.acm.org/citation.cfm?id=7929), p. 12, 2.1.3):

- send a finite number of messages to other actors (bsal_actor_send);
- create a finite number of new actors (bnsal_actor_spawn);
- designate the behavior to be used for the next message it receives (bsal_actor_pointer, bsal_actor_die).

Other names for the actor model: actors, virtual processors, activation frames, streams
([Hewitt, Bishop, Steiger 1973](http://dl.acm.org/citation.cfm?id=1624804)).

Also, in the actor model, the arrival order of messages is both arbitrary and unknown
([Agha 1986](http://dl.acm.org/citation.cfm?id=7929), p. 22, 2.4).

## Actor model links

- [Why say Actor Model instead of message passing?](http://lambda-the-ultimate.org/node/4683)
- [Actors: a model of concurrent computation in distributed systems](http://dl.acm.org/citation.cfm?id=7929)
- [A universal modular ACTOR for malism for artificial intelligence](http://dl.acm.org/citation.cfm?id=1624804)
- [Actor model](http://en.wikipedia.org/wiki/Actor_model)

## Languages using actors

- [Erlang](http://www.erlang.org/)
- [Scala](http://www.scala-lang.org/)
- [D](http://dlang.org/)

# Design

Key concepts

| Concept | Description | Structure |
| --- | --- | --- |
| Message | Information with a source and a destination | struct bsal_message |
| Actor | Something that receives messages and behaves according to a script | struct bsal_actor |
| Script | Describes the behavior of an actor ([Hewitt, Bishop, Steiger 1973](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.77.7898))| struct bsal_script |
| Work | 2-tuple with an actor and a message | struct bsal_work |
| Node | A runtime system that can be connected to other nodes (see [Erlang's definition](http://www.erlang.org/doc/reference_manual/distributed.html)) | struct bsal_node |
| Worker | An object that performs work for a living | bsal_worker |
| Queue | Each worker has a work queue and a message queue | struct bsal_fifo |
| Worker Pool | A set of available workers inside a node | struct bsal_worker_pool |

## Workflow (implementation of the runtime system)

The code has to be formulated in term of actors.
An actor has a name, and does something when it receives a message.
A message is however first received by a node. The node
prepares a work and gives it to a worker_pool. The worker pool
assigns the work to a worker. Finally, worker eventually calls
the corresponding receive function using the actor and message presented inside
the work.

## Runtime

The number of nodes is set by mpiexec -n @number_of_bsal_nodes ./a.out.
The number of threads on each node is set with -threads-per-node.

The following command starts 256 nodes (there is 1 MPI rank per
node) and 64 threads per node for a total of
256 * 64 = 16384 threads.

```bash
mpiexec -n 256 ./a.out -threads-per-node 64
```

Because the whole thing is event-driven by inbound and outbound messages,
a single node can run much more actors than the number of
threads it has.

The runtime also supports asymmetric numbers of threads:

```bash
# launch 32 nodes with 32 threads, 244 threads, 32 threads, 244 threads, and so on
mpiexec -n 32 ./a.out -threads-per-node 32,244
```

## Design links

- [Linux workqueue](https://www.kernel.org/doc/Documentation/workqueue.txt)

# Other possible names

- name := biosal
- bio := Biological or Biology
- s := Sequence or Scalable or Salable or Salubrious or Satisfactory
- a := Analysis or Actor or Actors
- l := Library or Lift

Alternative name: BIOlogy Scalable Actor Library

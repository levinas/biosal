
examples: mock mock1 ring reader not_found remote_spawn synchronize controller

EXAMPLES=example_mock example_ring example_reader example_remote_spawn example_synchronize example_controller

# examples
EXAMPLE_MOCK=examples/mock/main.o examples/mock/mock.o examples/mock/buddy.o
EXAMPLE_RING=examples/ring/main.o examples/ring/sender.o examples/ring/ring.o
EXAMPLE_READER=examples/reader/main.o examples/reader/reader.o
EXAMPLE_REMOTE_SPAWN=examples/remote_spawn/main.o examples/remote_spawn/table.o
EXAMPLE_SYNCHRONIZE=examples/synchronize/main.o examples/synchronize/stream.o
EXAMPLE_CONTROLLER=examples/controller/main.o examples/controller/root.o

ring: example_ring
	mpiexec -n 2 ./example_ring -threads-per-node 8

mock: example_mock
	mpiexec -n 3 ./example_mock -threads-per-node 7

mock1: example_mock
	mpiexec -n 3 ./example_mock -threads-per-node 1

reader: example_reader
	mpiexec -n 2 ./example_reader -threads-per-node 13 -read ~/dropbox/GPIC.1424-1.1371.fastq

synchronize: example_synchronize
	mpiexec -n 3 ./example_synchronize -threads-per-node 9

controller: example_controller
	mpiexec -n 3 ./example_controller -threads-per-node 9 ~/dropbox/*.fastq

remote_spawn: example_remote_spawn
	mpiexec -n 6 ./example_remote_spawn -threads-per-node 1,2,3

not_found: example_reader
	mpiexec -n 3 ./example_reader -threads-per-node 7 -read void.fastq


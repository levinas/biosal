#!/bin/bash

#-debug-memory-pools \
#-enable-actor-load-profiler -enable-transport-profiler

mpiexec -n 4 ./applications/spate_metagenome_assembler/spate -k 33 -threads-per-node 8 ~/dropbox/S.aureus.fasta.gz \
    -print-thorium-data -freopen-stdout -o spate-007

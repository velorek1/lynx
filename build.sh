#!/bin/bash
gcc -c rterm.c
gcc -c tm.c
gcc -c scbuf.c
gcc -c demo.c
gcc demo.o tm.o rterm.o scbuf.o -o demo

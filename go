#!/bin/sh
make clean; wwwctl stop; make; make install; wwwctl startssl; make clean;

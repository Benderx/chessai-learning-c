#!/bin/bash
make clean
make

if [ $? -eq 0 ]
then
	echo "make ran successfully"
	./play
	make clean
else
	echo "did not run due to makefile errors"
fi

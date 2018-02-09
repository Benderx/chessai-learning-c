#!/bin/bash
make

if [ $? -eq 0 ]
then
	echo "make ran successfully"
	./driver
	make clean
else
	echo "did not run due to makefile errors"
fi
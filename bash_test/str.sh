#!/bin/sh 

# test string 

str=HELLO

if [ str ]; then
	echo "str len > 0"
else
	echo "str len < 0"
fi

str1=HELLO

if [ $str = $str1 ]; then 
	echo "equal"
else
	echo "not equal"
fi

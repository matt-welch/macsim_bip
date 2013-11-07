#!/bin/bash
# filename: make_trace_files
for i in $(cat all_traces.txt); 
do 
	echo
	export CLASS=$(echo $i | sed -r 's:/home/jwelch4/cse520/research/macsim/trace/x86/::' | sed -r 's/([a-zA-Z0-9]*).*/\1/')
	export FNAME=$(echo $i | sed -r 's:/home/jwelch4/cse520/research/macsim/trace/x86/spec([0-9]*[a-z]*)/::' | sed -r 's/([a-zA-Z0-9]*).*/\1/')
	echo 1 > trace_file_$CLASS-$FNAME
	echo $i >> trace_file_$CLASS-$FNAME
	echo "trace_file_$CLASS-$FNAME = "
	cat trace_file_$CLASS-$FNAME
done

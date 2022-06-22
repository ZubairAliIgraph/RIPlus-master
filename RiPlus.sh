#!/bin/bash
#DataGraphPath=./ReferenceGraph/human.txt
#DataGraphPath=./ReferenceGraph/yeast.txt
DataGraphPath=./ReferenceGraph/hprd
#DataGraphPath=./ReferenceGraph/RunningExampleData.igraph

NumberofQueryGraph=100
cp /dev/null output
	echo "Reference graph $DataGraphPath"
	for (( c=0; c<$NumberofQueryGraph; c++ ))
	do  
		#QueryGraphPath=./QueryGraph/humanQueries/q25/q$c.igraph >> output
		#QueryGraphPath=./QueryGraph/yeastQueries/q25/q$c.igraph >> output
		QueryGraphPath=./QueryGraph/hprdQueries/q25/q$c.igraph >> output
		#QueryGraphPath=./QueryGraph/RunningExampleQuery.igraph
		echo "Query graph $QueryGraphPath"
		
		#timeout 10s ./RiIgraph $QueryGraphPath $DataGraphPath >> output
#		timeout 10s ./RiIgraph $QueryGraphPath $DataGraphPath ||  echo Straggler 

		timeout 10s ./RiPlus $QueryGraphPath $DataGraphPath >> output
#		timeout 10s ./RiPlus $QueryGraphPath $DataGraphPath ||  echo Straggler 
	done
echo "Total Search Time"
grep "matching time" output|awk '{ SUM += $3} END { print SUM }'
echo "Total match found"
grep "number of found matches" output|awk '{ SUM += $5} END { print SUM }'




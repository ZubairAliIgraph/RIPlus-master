# RIPlus-master
RI+ is an enhanced algorithm designed for induced and non-induced form of subgraph matching problem.
# To compile RI+
To compile RI+, you need gcc compiler, source code of RI (https://github.com/InfOmics/RI), and igraph library (visit https://igraph.org/c/)
Install these and compile RI
Compile RiPlus.cpp as g++ RiPlus.cpp -I/usr/local/include/igraph/ -L/usr/local/lib -ligraph -o RiPlus
if some problem in compiling visit  https://igraph.org/c/
# To Run RI+
$ ./RiPlus.sh

# parallel-universe
Computer Science Senior Capstone Project


This Computer Science Senior Capstone Project C file leverages high-performance computing (HPC) 
techniques via the Message-Passing Interface (MPI) to attempt to increase and 
monitor CPU temperatures to maintain a specific overall temperature of a node cluster.

This software is being written for a small-scale Raspberry Pi node cluster 
in a proof-of-concept format.

Data collected will be visualized to extrapolate on the potential of using a computing cluster,
such as a data center, to provide a relatively-consistent baseline-level of heat output
for productive re-use, while hopefully not significantly impacting the performance of 
the data center itself. Percentage of acceptable temperature fluctuation is 
yet-to-be-determined.

CPUs will be maxed-out through a custom calculation function to keep CPUs loaded with jobs,
and temperature data will be recorded after each calculation. A goal is to set a temperature limit
on the CPUs to pause calculations when temperatures veer outside of the acceptable range.
This would allow for a predictable temperature output range under maximum CPU load, regardless 
of requested processing power by outside network clients. This, in turn, would demonstrate practicality
by way of a predictable temperature output in the use of this concept as a real-world heating tool. 

Data will be visualized using the Ganglia open-source software package.

----------------------------------

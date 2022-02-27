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

INSTRUCTIONS:

to compile and run, MPI 3.1.1 is recommended (software built with this version of MPI).

This is built to be run on a Raspberry Pi cluster, and was tested using one from the CSinParallel program. Therefore,
as the CSinParallel system had a hostfile on it to recognize all available nodes and processor cores for running, the instructions below assume you are in posession of the hostfile from this device. If not, the file will, by default, run on a single node with "mpirun main".

In Terminal (macOS/Ubuntu):

1. cd ...parallel-universe/src/
2. make
3. mpirun -hostfile ~/hostfile -np [number of cores] ./main

2/27/22.
Code written by Brendan T. Pritikin. All Rights Reserved.
----------------------------------

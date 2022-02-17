#include <math.h> // include math library
#include <stdio.h> // inlcude standard IO library
#include <stdlib.h> //include standard library
#include <time.h> // include time library.
#include <mpi.h> // include MPI functionality.

int main(int argc, char *argv[]){
    

printf("Program set-up completed. Loaded: \n1. math.h\n2. stdio.h\n3. stdlib.h\n4. time.h\n");

// initialize MPI
MPI_Init(NULL, NULL);  

//recall, everything in this program will be run once by each
// processor, including all print statements.

//Get basic data about system
// 1. Number of processes (cores/processors available).
// 2. Rank of processors.
// 3. Name of processors.
// 4. Current temperature of processors (raspberry pi-specific).

int world_size;
MPI_Comm_size(MPI_COMM_WORLD, &world_size); //get numnber of processes.

int world_ranking;
MPI_Comm_rank(MPI_COMM_WORLD, &world_ranking); //get rank of each processor (processor #)

char core_name[MPI_MAX_PROCESSOR_NAME];
int name_length;
MPI_Get_processor_name(core_name, &name_length);

// Test hello message
printf("hello from the computer named %s, core %d out of %d.\n", 
            core_name, world_ranking, world_size);

//get temperature from each core on the Pi:

FILE *temperatureRecord; //open Temp file from Pi.
double Temperature;
temperatureRecord = fopen("/sys/class/thermal/thermal_zone0/temp", "r"); //read CPU temp. data from Pi.

fscanf(temperatureRecord, "%lf", &Temperature);
Temperature = (((Temperature / 1000) * 9) / 5); // convert from mili-degrees to Fahrenheit.

printf("TEST PRINT OF RAW TEMPERATURE DATA:\n\n%f", Temperature);

// if(piCoreNum.temp is too high, delay cycle. Else, run bcast (mayebe not bcast as
//        that broadcasts to ALL cores code again.)

MPI_Finalize(); //Finalize MPI environment.
}

#include <math.h> // include math library
#include <stdio.h> // inlcude standard IO library
#include <stdlib.h> //include standard library
#include <time.h> // include time library.
#include <mpi.h> // include MPI library.


/*
 * This Senior Capstone Project C file leverages 
 * High-Performance Computing techniques via the
 * Message-Passing Interface (MPI) to attempt to increase
 * and monitor CPU temperatures to maintain a specific reading.
 * 
 * This software is being written for a small-scale Raspberry Pi
 * node cluster in a proof-of-concept format. The goal of this
 * project is to max-out the CPU abilities on each available core
 * (processs), simulating a fully-loaded data center node, and
 * later analyze this data to extrapolate the abilities of a
 * data center cluster to provide a baseline-level of heat output
 * which has little fluctuation. Percentage of acceptable 
 * fluctuation is yet-to-be-determined.
 * 
 * CPU temperature data will be recorded, and tests MAY be run
 * with an arbitrary maximum CPU temperature limit having been
 * be set on the Pi nodes in use to attempt to collect 
 * data on whether a consistent MAX temperature output 
 * from nodes is possible. This promotes the idea that
 * data centers could be relied-upon for a MINIMUM as well as a
 * MAXIMUM standard heat range, regardless of
 * requested processing power by outside network clients.
 * 
 * Data is to-be-visualized using the Ganglia open-source 
 * software package.
 * 
 */


/* Author: Brendan T. Pritikin */
/* Thesis Advisor: Prof. John Rieffel */
/* Institution: Union College, Schenectady, NY */
/* Last Modified: February 19, 2022 */


/**
 * @brief squares two doubles.
 * 
 * @param randNum1 pre-generated random number.
 * @param randNum2 pre-generated random number.
 * @return double squared value.
 */
double square(double randNum1, double randNum2)
{
    double squaredNum = (randNum1 * randNum2);
    
    return squaredNum;
}


/**
 * @brief gets current core/process temp data from Raspberry Pi Nodes.
 * 
 * @return double current temperature from each node, core-by-core.
 */
double getCoreTemps()
{
    FILE *temperatureRecord; //open Temp file from Pi.
    double currentTemperature; //core temperature data.
    temperatureRecord = fopen("/sys/class/thermal/thermal_zone0/temp", "r"); //read CPU temp. data from Pi.

    fscanf(temperatureRecord, "%lf", &currentTemperature);
    currentTemperature = (((currentTemperature / 1000) * 9) / 5); // convert from mili-degrees to Fahrenheit.

    return currentTemperature;

}






int main(int argc, char** argv){


double randNum1; //for squaring fcn.
double randNum2; //for squaring fcn.
// double squareResult; //stores squared value to run-up processor core temps via arbitrary work.
int introMessageDisplayed = 0; //allows display of informational message upon mpirun.






// initialize MPI
MPI_Init(&argc, &argv); //sets up MPI. Do not alter.

int number_of_cores; //for MPI testing.
int core_number; //for MPI testing.
char system_name[MPI_MAX_PROCESSOR_NAME]; //for MPI testing.
int sys_name_char_length; //for MPI testing

randNum1 = ((unsigned)(time(NULL))); //get unix timestamp for first random number.
randNum2 = arc4random_uniform(100); //generate "random" number between 0 and 100.
double currentTemperature;


//BEGIN MPI TESTING.

//recall, everything in this program will be run once by each
// processor, including all print statements.

//Get basic data about system
// 1. Number of processes (cores/processors available).
// 2. Rank of processors.
// 3. Name of processors.
// 4. Current temperature of processors (raspberry pi-specific).

MPI_Comm_size(MPI_COMM_WORLD, &number_of_cores); //get number of processes.
MPI_Comm_rank(MPI_COMM_WORLD, &core_number); //get rank of each processor (processor #)
MPI_Get_processor_name(system_name, &sys_name_char_length);


// Test hello message
//printf("\n\nhello from the computer named %s, core %d out of %d.\n\n", 
//            system_name, core_number, number_of_cores);



//END MPI TESTING.


if(introMessageDisplayed == 0 && core_number == 0){
    printf("-----------------------------------------\n");
    printf("PROGRAM INFORMATION: \n\nLoaded: \n1. math.h\n2. stdio.h\n3. stdlib.h\n4. time.h\n5. mpi.h\n\n");

    printf("This program will calculate two pseudo-random numbers and will then square them.\n");
    printf("The process will continue until a CPU temperature threshold has been set,\n");
    printf("at which time the program will PAUSE until the temperature has fallen below\n");
    printf("the threshold. Then, squaring of numbers will continue.\n");
    printf("-----------------------------------------\n\n\n\n\n");

    introMessageDisplayed = 1;
}


printf("\n------------------------------------------\n");

/**
 * @brief display core temp once in output.
 * 
 */
if(core_number == 0)
{
    printf("Current core temperatures are: \n\n");
    currentTemperature = getCoreTemps();
    printf("%f", currentTemperature);

    printf("\n------------------------------------------\n");

}



// #FIXME if currentTemperature is too high, delay cycle. Else, run someting like an MPI_scatter/gather, etc.)

/**

if(core_number == 0)
{
    printf("test message from core 0.");
}

if(core_number == 1)
{
    printf("test message from core 1.");
}

if(core_number == 2)
{
    printf("test message from core 2.");
}

if(core_number == 3)
{
    printf("test message from core 3.");
}

*/

MPI_Finalize(); //Finalize MPI environment.
return 0;

}

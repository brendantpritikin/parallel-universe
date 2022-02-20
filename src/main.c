#include <math.h> // include math library
#include <stdio.h> // inlcude standard IO library
#include <stdlib.h> //include standard library
#include <time.h> // include time library.
#include <mpi.h> // include MPI library.


/*
 * This Computer Science Senior Capstone Project C file leverages 
 * high-performance computing (HPC) techniques via the Message-Passing 
 * Interface (MPI) to attempt to increase and monitor CPU temperatures 
 * to maintain a specific overall temperature of a node cluster.
 *
 * This software is being written for a small-scale Raspberry Pi node cluster 
 * in a proof-of-concept format.
 * 
 * Data collected will be visualized to extrapolate on the potential of using a 
 * computing cluster, such as a data center, to provide a relatively-consistent 
 * baseline-level of heat output for productive re-use, while hopefully not 
 * significantly impacting the performance of the data center itself. Percentage of
 * acceptable temperature fluctuation is yet-to-be-determined.
 * 
 * CPUs will be maxed-out through a custom calculation function to keep CPUs loaded
 * with jobs, and temperature data will be recorded after each calculation. A goal 
 * is to set a temperature limit on the CPUs to pause calculations when temperatures 
 * veer outside of the acceptable range. This would allow for a predictable temperature
 * output range under maximum CPU load, regardless of requested processing power by outside
 * network clients. This, in turn, would demonstrate practicality by way of a predictable 
 * temperature output in the use of this concept as a real-world heating tool.
 * 
 * Data will be visualized using the Ganglia open-source software package.
 */


/* Author: Brendan T. Pritikin */
/* Thesis Advisor: Prof. John Rieffel */
/* Institution: Union College, Schenectady, NY */
/* Last Modified: February 20, 2022 */


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
 * @brief gets current CPU temp data from a single Raspberry Pi Node.
 * 
 * @return double current CPU temp for a single node.
 */
double get_CPU_Temp()
{
    FILE *temperatureRecord; //open Temp file from Pi.
    double currentTemperature; //core temperature data.
    temperatureRecord = fopen("/sys/class/thermal/thermal_zone0/temp", "r"); //read CPU temp. data from Pi.

    fscanf(temperatureRecord, "%lf", &currentTemperature);
    currentTemperature = (((currentTemperature / 1000) * 9) / 5); // convert from mili-degrees to Fahrenheit.

    return currentTemperature;
}

/**
 * @brief get temperature of each node's CPU, add to array.
 * 
 * @param number_of_cores_total 
 *  
 */
void save_all_node_temps(int number_of_cores_total, double** node_temp_record_array, int next_save_index)
{
    for(int core = 0; core < (number_of_cores_total-3); core+=4)
    {
        double currentTemperature = get_CPU_Temp();
        printf("Node %d CPU temp: %f\n", (core/4), currentTemperature);
        node_temp_record_array[core][next_save_index] = currentTemperature;
        printf("Temp saved.------\n");
    }
}

/**
 * @brief create an array to store node CPU temp recordings.
 * 
 * @param num_cores_total total cores, to use for sizing to # of nodes.
 * @param number_of_temp_recordings total recordings to capture for program run.
 * @return double* array to store data in.
 */
double* temperature_storage_array(int num_cores_total, int number_of_temp_recordings)
{
    double *temp_array = (double *) malloc((num_cores_total/4) * number_of_temp_recordings * sizeof(int)); //holds each node's temps in a 2D array.
    return temp_array; // this should return a pointer to the beginning of the array.
}



int main(int argc, char** argv){


double randNum1; //for squaring fcn.
double randNum2; //for squaring fcn.
// double squareResult; //stores squared value to run-up processor core temps via arbitrary work.
int introMessageDisplayed = 0; //allows display of informational message upon mpirun.
int num_of_temp_recordings = 1000; //number of temperature records to store for each node.
int current_recording_iteration = 0; //holds current iteration count of temp recording.
int number_of_cores_outside_MPI_declr = 0;
double* node_temp_record = temperature_storage_array(number_of_cores_outside_MPI_declr, num_of_temp_recordings);
const double TEMPERATURE_THRESHOLD = 70.0; //maximum °F setting for CPUs, i.e. temperature limit.


// initialize MPI
MPI_Init(&argc, &argv); //sets up MPI. Do not alter.

int number_of_cores; //for MPI testing.
int core_number; //for MPI testing.
char system_name[MPI_MAX_PROCESSOR_NAME]; //for MPI testing.
int sys_name_char_length; //for MPI testing

randNum1 = ((unsigned)(time(NULL))); //get unix timestamp for first random number.
randNum2 = rand() % (100 + 1 - 1) + 1; //generate "random" number between 1 and 100. (.. % max_num + 1 - min_num) + min_num.
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
printf("\n\nhello from the computer named %s, core %d out of %d.\n\n", 
            system_name, core_number, number_of_cores);



//END MPI TESTING.

//#FIXME - ADD MPI collective-communication for increasing efficiency in calculations...
//#FIXME - perhaps have an array of values from 1 to number_of_cores, and instruct each core to square the value in
//#FIXME - its individual node index until 1,000 operations have occurred?


number_of_cores_outside_MPI_declr = number_of_cores; //save non-MPI core count. Will be re-initialized with each MPI run. Minimal performance hit.


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

//#FIXME - OLD CODE FOR TEMPERATURE-PULLING. WILL DELETE AFTER TESTING NEW CODE.

/**
 * @brief display core temp once in output.
 * 
 */
if(core_number == 0) //node 0,core1 i.e. first core in first CPU.
{
    printf("OLD hard-coding FOR TESTING = node 0 CPU temp: \n\n");
    currentTemperature = get_CPU_Temp();
    printf("%f", currentTemperature);
    printf("\n------------------------------------------\n");
}

if(core_number == 4) //node 1
{
    printf("OLD hard-coding FOR TESTING = node 1 CPU temp: \n\n");
    currentTemperature = get_CPU_Temp();
    printf("%f", currentTemperature);
    printf("\n------------------------------------------\n");
}

if(core_number == 8) // node 2
{
    printf("OLD hard-coding FOR TESTING = node 2 CPU temp: \n\n");
    currentTemperature = get_CPU_Temp();
    printf("%f", currentTemperature);
    printf("\n------------------------------------------\n");
}


/**
 * @brief save currrent CPU temperatures from all connected nodes.
 * 
 */
if(current_recording_iteration < num_of_temp_recordings)
{
    save_all_node_temps(number_of_cores, &node_temp_record, current_recording_iteration);
}

for(int core = 0; core < number_of_cores-3; core+=4)
{
    if(node_temp_record[core][current_recording_iteration] > TEMPERATURE_THRESHOLD)
    {
        printf("oh no! Too hot to handle!"); // pause the program for a few seconds. #FIXME need to set this up.
    }
} 


//#FIXME: OLD CODE call/response from cores. WILL DELETE AFTER TESTING NEW CODE.

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

free(node_temp_record);

MPI_Finalize(); //Finalize MPI environment.
return 0;

}

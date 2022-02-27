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
 * in a proof-of-concept format, with the knowledge that each node has a quad-core
 * CPU.
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
 * 
 */


/* Author: Brendan T. Pritikin */
/* Thesis Advisor: Prof. John Rieffel */
/* Institution: Union College, Schenectady, NY */
/* Last Modified: February 27, 2022 */


/**
 * @brief squares two ints.
 * 
 * @param num_to_square number to square.
 * @return int squared value.
 */
int square(int num_to_square)
{
    int squaredNum = (num_to_square * num_to_square);
    
    return squaredNum;
}


/**
 * @brief gets current CPU temp data from a single Raspberry Pi Node.
 * 
 * @return double current CPU temp for a single node.
 */
double get_CPU_Temp()
{
    FILE *temperatureRecord; // open Temp file from Pi.
    double currentTemperature; // core temperature data.
    temperatureRecord = fopen("/sys/class/thermal/thermal_zone0/temp", "r"); // read CPU temp. data from Pi.

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
void save_all_node_temps(int number_of_cores_total, double** node_temp_record_array, int next_save_index_iteration)
{
    for(int core = 0; core < (number_of_cores_total-3); core+=4)
    {
        double currentTemperature = get_CPU_Temp();
        printf("Node %d CPU temp: %f\n", (core/4), currentTemperature);
        node_temp_record_array[next_save_index_iteration][core] = currentTemperature;
        printf("Temp saved.\n------\n"); 
    }
}



/**
 * @brief create an array ranging from the starting to the ending value passed-in (inclusive).
 * @return int* array full of values from start-to-finish.
 */
int * filled_value_array(int starting_value, int ending_value)
{
    static int int_array[1000000];

    for(int current_index = starting_value; current_index < 1000000; current_index++)
    {
        //printf("CURRENT INDEX TO STORE: %d", current_index);
        int_array[current_index] = current_index;
        //printf("INDEX STORED WAS: %d", int_array[current_index]);
    }
    return int_array;
}


/**
 * @brief create an array to store node CPU temp recordings.
 * 
 * @param num_cores_total total cores, to use for sizing to # of nodes.
 * @param number_of_temp_recordings total recordings to capture for program run.
 * @return double* array to store data in.
 */
double * temperature_storage_array(int num_cores_total, int number_of_temp_recordings)
{
    static double temp_array[1000000];
    return temp_array; // this should return a pointer to the beginning of the array.
}

/**
 * @brief stores result of MPI squaring operations. Filled with resultant squares squaring_range_start - squaring_range_end.
 * 
 * @param range_start first number to square (main-defined).
 * @param range_end last number to square (main-defined).
 * @return int* array from range_start to range_end
 */
int * resultant_data_storage_array(int range_start, int range_end)
{
    static int squaring_resultant_array[1000000];
    return squaring_resultant_array; // this should return a pointer to the beginning of the array.
}



int main(int argc, char* argv[]){

int number_of_cores; // for MPI testing.
int core_number; // for MPI testing.

int introMessageDisplayed = 0; // allows display of informational message upon mpirun.
int num_of_temp_recordings = 1000; // number of temperature records to store for each node.
int current_recording_iteration = 0; // holds current iteration count of temp recording.

double* node_temp_record;
const double TEMPERATURE_THRESHOLD = 65.0; // maximum °F setting for CPUs, i.e. temperature limit.
int num_range_start = 0; //
//int MPI_next_index_to_square = num_range_start; // stores value set above for MPI-specific use (just to keep things separate).
int num_range_end = num_of_temp_recordings; // enough room to hold the number of recordings planned.
// SQUARING RANGE START - SQUARING RANGE END MUST == N/P (tasks/processes). 
int* resultant_data_storage = resultant_data_storage_array(num_range_start, num_range_end); //holds all resulting squares calculated across nodes.
int* values_array_filled = filled_value_array(num_range_start, num_range_end);
int values_array_size = 1000000; //(num_range_end - num_range_start);
int* final_data_array = resultant_data_storage_array(num_range_start, num_range_end); // holds received values. from MPI_Recv.


//RESULTANT DATA STORAGE ARRAY MALLOC: (int *) malloc((range_end - range_start) * sizeof(int)); // 1-D array for holding squared value resultants.
//FILLED VALUE ARRAY MALLOC: (int *) malloc(starting_value * ending_value * sizeof(int)); // holds all ints to be loaded in, start-to-finish.
//TEMPERATURE STORAGE ARRAY (just for the heck of it as it has a malloc): (double *) malloc(number_of_temp_recordings * (num_cores_total/4) * sizeof(double)); //holds each node's temps in a 2D array.
//SETTING ALL TO A SIZE OF 1000000.



// initialize MPI
MPI_Init(&argc, &argv); // sets up MPI. Do not alter.

char system_name[MPI_MAX_PROCESSOR_NAME]; // for MPI testing.
int sys_name_char_length; // for MPI testing

//Get basic data about system
// 1. Number of processes (cores/processors available).
// 2. Rank of processors.
// 3. Name of processors.
// 4. Current temperature of processors (raspberry pi-specific).

MPI_Comm_size(MPI_COMM_WORLD, &number_of_cores); // get number of processes.
MPI_Comm_rank(MPI_COMM_WORLD, &core_number); // get rank of each processor (processor #)
MPI_Get_processor_name(system_name, &sys_name_char_length);


if(introMessageDisplayed == 0 && core_number == 0){
    printf("-----------------------------------------\n");
    printf("PROGRAM INFORMATION: \n\nLoaded: \n1. math.h\n2. stdio.h\n3. stdlib.h\n4. time.h\n5. mpi.h\n\n");

    printf("This program will calculate two pseudo-random numbers and will then square them.\n");
    printf("The process will continue until a CPU temperature threshold has been set,\n");
    printf("at which time the program will PAUSE until the temperature has fallen below\n");
    printf("the threshold. Then, squaring of numbers will continue.\n");
    printf("Temp threshold set to: %3.2f°F\n", TEMPERATURE_THRESHOLD);
    printf("-----------------------------------------\n\n\n\n\n");

    introMessageDisplayed = 1;
}


printf("\n------------------------------------------\n");

// create storage array for node temperature data.
node_temp_record = temperature_storage_array(number_of_cores, num_of_temp_recordings);



//WORKING SQUARING FUCTION CODE.
for(int current_index = 0; current_index < values_array_size; current_index++)
    {
        int temp;
        int temp2;
        temp = values_array_filled[current_index];
        temp2 = square(temp);
//        printf("\nCURRENT VALUE IS: %d", temp);
//        printf("SQUARED VALUE IS: %d\n", temp2);
        resultant_data_storage[current_index] = temp2; //store squred value in new array.
//        printf("\nSquared array value. Stored it. \nInitial value: %d, squared value: %d\n\n", temp, resultant_data_storage[current_index]);
    }


/**
 * @brief MPI_Send task/values_array_fllled values FROM all non-zero nodes TO node 0
 * for calculations to take place.
 * 
 */
if(core_number != 0)
{
    MPI_Send(resultant_data_storage, 1000000, MPI_INT, 0, 0, MPI_COMM_WORLD); // MPI_Send-to-core-0-operation.
}

/**
 * @brief MPI_Receive tasks from all non-zero nodes to node 0 (assuming quad-core).
 *          Goal: square next value in values_array_filled and store result in 
 *                  resultant_data_storage.
 * 
 */
if(core_number == 0)
{
    int localArray[1000000];

    /**
     * @brief receive data into a local array.
     * 
     */
    for(int core_recv_from = 1; core_recv_from < number_of_cores; core_recv_from++)
    {
        MPI_Recv(localArray, 1000000, MPI_INT, core_recv_from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    /**
     * @brief move data to final array for output.
     * 
     */
    for(int i = 0; i < 1000000; i++)
    {
        final_data_array[i] = localArray[i];
    }

    for(int array_index = 0; array_index < 1000000; array_index++)
    {
        printf("-------Collective calculations finished. The perfect square of %d is: %d\n", values_array_filled[array_index], final_data_array[array_index]);
    }
}

/**
 * @brief save currrent CPU temperatures from all connected nodes.
 * 
 */
if(current_recording_iteration < num_of_temp_recordings)
{
    save_all_node_temps(number_of_cores, &node_temp_record, current_recording_iteration);
}

/**
 * @brief print current CPU temperature. Print overtemp temperature
 *  repeatedly (pause calculations by staying in this loop) until temp drops
 *  below prescribed level as set in TEMPERATURE_THRESHOLD.
 * 
 */
for(int core = 0; core < number_of_cores-3; core+=4)
{
    //row-major format - i = current iteration/row; ncols = CPU # (assuming quad-core cpu); j = column/cpu temp.
    printf("CPU %d temp is currently: %f°F\n", core, node_temp_record[current_recording_iteration * (number_of_cores/4) + core]);

    while(node_temp_record[current_recording_iteration * (number_of_cores/4) + core]  > TEMPERATURE_THRESHOLD)
    {
        printf("SUCCESS. TEMPERATURE THRESHOLD REACHED. Waiting for CPUs to cool\n");
        printf("just below threshold before continuing calcuations to maintain a consistent average temperature.");
        printf("CPU %d temp is %f and limit is set at %f.", core, node_temp_record[current_recording_iteration * (number_of_cores/4) + core], TEMPERATURE_THRESHOLD);
    }
} 

if(core_number == 0)
{
//    printf("\n\nFilled value array looks like this: %d.", *values_array_filled); // numbers before squaring.
//    printf("\n\nresulting data looks like this: %d", *final_data_array); //squared number results.
    printf("\n\nHere is the array of collected temperature data: %f.\n", *node_temp_record); //node temp data.
}

//free(node_temp_record); //not necessary unless using malloc().
//free(values_array_filled); //not necessary unless using malloc().
//free(resultant_data_storage); //not necessary unless using malloc().
//free(final_data_array); //not necessary unless using malloc().

MPI_Finalize(); //Finalize MPI environment.

return 0;

}

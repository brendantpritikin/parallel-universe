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
/* Last Modified: February 25, 2022 */


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
int* filled_value_array(int starting_value, int ending_value)
{
    int *int_array = (int *) malloc(starting_value * ending_value * sizeof(int)); // holds all ints to be loaded in, start-to-finish.

    for(int current_index = starting_value; current_index <= ending_value; current_index++)
    {
        int_array[current_index] = current_index;
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
double* temperature_storage_array(int num_cores_total, int number_of_temp_recordings)
{
    double *temp_array = (double *) malloc(number_of_temp_recordings * (num_cores_total/4) * sizeof(double)); //holds each node's temps in a 2D array.
    return temp_array; // this should return a pointer to the beginning of the array.
}

/**
 * @brief stores result of MPI squaring operations. Filled with resultant squares squaring_range_start - squaring_range_end.
 * 
 * @param range_start first number to square (main-defined).
 * @param range_end last number to square (main-defined).
 * @return int* array from range_start to range_end
 */
int* resultant_data_storage_array(int range_start, int range_end)
{
    int *squaring_resultant_array = (int *) malloc((range_end - range_start) * sizeof(int)); // 1-D array for holding squared value resultants.
    return squaring_resultant_array; // this should return a pointer to the beginning of the array.
}



int main(int argc, char** argv){


double randNum1; // for squaring fcn.
double randNum2; // for squaring fcn.
// double squareResult; //stores squared value to run-up processor core temps via arbitrary work.
int introMessageDisplayed = 0; // allows display of informational message upon mpirun.
int num_of_temp_recordings = 1000; // number of temperature records to store for each node.
int current_recording_iteration = 0; // holds current iteration count of temp recording.
int number_of_cores_outside_MPI_declr = 0;

double* node_temp_record = temperature_storage_array(number_of_cores_outside_MPI_declr, num_of_temp_recordings);
const double TEMPERATURE_THRESHOLD = 70.0; // maximum °F setting for CPUs, i.e. temperature limit.
int num_range_start = 0; //
int MPI_next_index_to_square = num_range_start; // stores value set above for MPI-specific use (just to keep things separate).
int num_range_end = num_of_temp_recordings; // enough room to hold the number of recordings planned.
// SQUARING RANGE START - SQUARING RANGE END MUST == N/P (tasks/processes). 
int* resultant_data_storage = resultant_data_storage_array(num_range_start, num_range_end); //holds all resulting squares calculated across nodes.
int* values_array_filled = filled_value_array(num_range_start, num_range_end);



// initialize MPI
MPI_Init(&argc, &argv); // sets up MPI. Do not alter.

int number_of_cores; // for MPI testing.
int core_number; // for MPI testing.
char system_name[MPI_MAX_PROCESSOR_NAME]; // for MPI testing.
int sys_name_char_length; // for MPI testing

randNum1 = ((unsigned)(time(NULL))); // get unix timestamp for first random number.
randNum2 = rand() % (100 + 1 - 1) + 1; // generate "random" number between 1 and 100. (.. % max_num + 1 - min_num) + min_num.
double currentTemperature;


//BEGIN MPI TESTING.

//recall, everything in this program will be run once by each
// processor, including all print statements.

//Get basic data about system
// 1. Number of processes (cores/processors available).
// 2. Rank of processors.
// 3. Name of processors.
// 4. Current temperature of processors (raspberry pi-specific).

MPI_Comm_size(MPI_COMM_WORLD, &number_of_cores); // get number of processes.
MPI_Comm_rank(MPI_COMM_WORLD, &core_number); // get rank of each processor (processor #)
MPI_Get_processor_name(system_name, &sys_name_char_length);


// Test hello message
printf("\n\nhello from the computer named %s, core %d out of %d.\n\n", 
            system_name, core_number, number_of_cores);



//END MPI TESTING.

//#FIXME - ADD MPI collective-communication for increasing efficiency in calculations...
//#FIXME - perhaps have an array of values from 1 to number_of_cores, and instruct each core to square the value in
//#FIXME - its individual node index until 1,000 operations have occurred?
    //YES - ARRAY IS CALLED RESULTANT_DATA_STORAGE_ARRAY. IT IS 1-D.    

// So, working idea now is to implement the following: the calculation we can do with MPI_Recv (and MPI_Send) is given a required calculation over a
// certain distance/range (say, collectively we want a list of every square from 1-1,000,000,000 [inclusive]). To accomplish this more efficiently than serially,
// we can pass work split up in the format of n/p (tasks/processors available)
// to every available process discovered at MPI_Init time. 
//So, we take the number of tasks (1,000,000,000 - 1 tasks) and the number of processes P
// (which, for any dual-core or higher system will be evenly-divisible), separate out the work by # of tasks, evenly,
// compute, and return as the values are calculated. Given MPI_Recv and MPI_Send, processes will return as they have completed,
// not in an orderly fashion, but that's OK. Goal is to compute everything REQUIRED as QUICKLY as possible, with the benefit
// of parallel computing to accelerate an otherwise slow, slow task.


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


//add MPI task distribution here.
//calculations will be performed via collective-communication.


/**
 * @brief MPI_Send task FROM all non-zero nodes TO node 0.
 * 
 */
if(core_number == 0)
{
    MPI_Send(&values_array_filled, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // MPI_Send-to-core-0-operation.
}

/**
 * @brief MPI_Receive tasks from all non-zero nodes to node 0 (assuming quad-core).
 * 
 */
for(int core = 4; core < (number_of_cores/4); core+=4)
{
    int squared_value;
    MPI_Recv(&squared_value, 1, MPI_INT, core, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //add to resultant data storage array. 
    resultant_data_storage[MPI_next_index_to_square] = squared_value;
    MPI_next_index_to_square += 1; // move counter to next index.
}



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
    // [core][current_recording_iteration] old array-iterating code.
    //row-major format - i = current iteration/row; ncols = CPU # (assuming quad-core cpu); j = column/cpu temp.
    while(node_temp_record[current_recording_iteration * (number_of_cores/4) + core]  > TEMPERATURE_THRESHOLD)
    {
        printf("SUCCESS. TEMPERATURE THRESHOLD REACHED. Waiting for CPUs to cool\n");
        printf("just below threshold before continuing calcuations to maintain a consistent average temperature.");
        printf("CPU %d temp is %f and limit is set at %f.", core, node_temp_record[current_recording_iteration * (number_of_cores/4) + core], TEMPERATURE_THRESHOLD);
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
free(values_array_filled);
free(resultant_data_storage);

MPI_Finalize(); //Finalize MPI environment.
return 0;

}

#pragma once

#ifdef __cplusplus  
extern "C" {
#endif 

#include <time.h>
#include <stdbool.h>
#include <pspkerneltypes.h>
#include <psptypes.h>
#include <psprtc.h>


/**
* The mode of execution for a specific job.
*/
#define MELIB_EXEC_DEFAULT	0x0 /** Executes on the ME, unless Dynamic Rebalancing is turned on */
#define MELIB_EXEC_CPU		0x1 /**  Executes specifically on the main CPU, regardless of job mode. Always runs synchronously (gives a small delay between jobs).*/
#define MELIB_EXEC_ME		0x2 /** Executes specifically on the Media Engine, regardless of job mode. Always runs asynchronously.*/


/**
* This structure is used to determine job execution.
* Given the properties in this structure, the manager will execute correctly
*/
struct JobInfo {
	unsigned char id; /** This ID is purely useless for now - but may be used in the future for performance tracking */
	unsigned char execMode; /** Uses execution mode to specify where the code will run and/or if said code can be dynamically rebalanced */
};

/**
* Job Data is an integer pointer to an address with the data.
*/
typedef int JobData;
	
/**
* This typedef defines a JobFunction as an integer function with given data.
*/
typedef int (*JobFunction)(JobData ptr);

/**
* This structure is used to give job information alongside the job itself and the data needed.
*/
struct Job {
	JobFunction function;
	JobData data;
	struct JobInfo jobInfo;
};

/** 
* JobManager class. This class only can have one instance for the ME.
*/

int J_Init(bool dynamicRebalance); /** Initialize the job manager with the option to dynamically rebalance loads. */
void J_Cleanup(); /** Cleans up and ends execution. */

void J_AddJob(struct Job* job); /** Adds a job to the queue. If the queue is full (max 256) then it will force a dispatch before adding more. */
void J_ClearJob(); /** Clears and deletes all jobs */

void J_DispatchJobs(float cpuTime); /** Starts a thread to dispatch jobs and execute! CPU Time is the time used by the rest of the system for Dynamic Rebalancing - it's unused otherwise. */
void J_Update(float cpuTime); /** Dispatches and runs jobs on this thread. See above for CPU Time */

float J_GetMETime();
float J_GetCPUTime();

struct me_struct* mei;

#ifdef __cplusplus  
}
#endif 
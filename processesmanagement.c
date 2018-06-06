
/*****************************************************************************\
* Laboratory Exercises COMP 3500                                              *
* Author: Saad Biaz                                                           *
* Updated 5/22/2017 to distribute to students to do Lab 1                     *
\*****************************************************************************/

/*****************************************************************************\
*                             Global system headers                           *
\*****************************************************************************/


#include "common.h"

/*****************************************************************************\
*                             Global data types                               *
\*****************************************************************************/

typedef enum {TAT,RT,CBT,THGT,WT} Metric;


/*****************************************************************************\
*                             Global definitions                              *
\*****************************************************************************/
#define MAX_QUEUE_SIZE 10
#define FCFS            1
#define SRTF            2
#define RR              3


#define MAXMETRICS      5



/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/




/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/

Quantity NumberofJobs[MAXMETRICS]; // Number of Jobs for which metric was collected
Average  SumMetrics[MAXMETRICS]; // Sum for each Metrics

/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/

void                 ManageProcesses(void);
void                 NewJobIn(ProcessControlBlock whichProcess);
void                 BookKeeping(void);
Flag                 ManagementInitialization(void);
void                 LongtermScheduler(void);
void                 IO();
void                 CPUScheduler(Identifier whichPolicy);
ProcessControlBlock *SRTF_Scheduler();
ProcessControlBlock *FCFS_Scheduler();
ProcessControlBlock *RR_Scheduler();
void                 Dispatcher();

/*****************************************************************************\
* function: main()                                                            *
* usage:    Create an artificial environment operating systems. The parent    *
*           process is the "Operating Systems" managing the processes using   *
*           the resources (CPU and Memory) of the system                      *
*******************************************************************************
* Inputs: ANSI flat C command line parameters                                 *
* Output: None                                                                *
*                                                                             *
* INITIALIZE PROGRAM ENVIRONMENT                                              *
* START CONTROL ROUTINE                                                       *
\*****************************************************************************/

int main (int argc, char **argv) {
   if (Initialization(argc,argv)){
     ManageProcesses();
   }
} /* end of main function */

/***********************************************************************\
* Input : none                                                          *
* Output: None                                                          *
* Function: Monitor Sources and process events (written by students)    *
\***********************************************************************/

void ManageProcesses(void){
  ManagementInitialization();
  while (1) {
    IO();
    CPUScheduler(PolicyNumber);
    Dispatcher();
  }
}

/* XXXXXXXXX Do Not Change IO() Routine XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/***********************************************************************\
* Input : none                                                          *
* Output: None                                                          *
* Function:                                                             *
*    1) if CPU Burst done, then move process on CPU to Waiting Queue    *
*         otherwise (for RR) return Process to Ready Queue              *
*    2) scan Waiting Queue to find processes with complete I/O          *
*           and move them to Ready Queue                                *
\***********************************************************************/
void IO() {
  ProcessControlBlock *currentProcess = DequeueProcess(RUNNINGQUEUE);
  if (currentProcess){
    if (currentProcess->RemainingCpuBurstTime <= 0) { // Finished current CPU Burst
      currentProcess->TimeEnterWaiting = Now(); // Record when entered the waiting queue
      EnqueueProcess(WAITINGQUEUE, currentProcess); // Move to Waiting Queue
      currentProcess->TimeIOBurstDone = Now() + currentProcess->IOBurstTime; // Record when IO completes
      currentProcess->state = WAITING;
    } else { // Must return to Ready Queue
      currentProcess->JobStartTime = Now();
      EnqueueProcess(READYQUEUE, currentProcess); // Mobe back to Ready Queue
      currentProcess->state = READY; // Update PCB state
    }
  }

  /* Scan Waiting Queue to find processes that got IOs  complete*/
  ProcessControlBlock *ProcessToMove;
  /* Scan Waiting List to find processes that got complete IOs */
  ProcessToMove = DequeueProcess(WAITINGQUEUE);
  if (ProcessToMove){
    Identifier IDFirstProcess =ProcessToMove->ProcessID;
    EnqueueProcess(WAITINGQUEUE,ProcessToMove);
    ProcessToMove = DequeueProcess(WAITINGQUEUE);
    while (ProcessToMove){
      if (Now()>=ProcessToMove->TimeIOBurstDone){
	ProcessToMove->RemainingCpuBurstTime = ProcessToMove->CpuBurstTime;
	ProcessToMove->JobStartTime = Now();
	EnqueueProcess(READYQUEUE,ProcessToMove);
      } else {
	EnqueueProcess(WAITINGQUEUE,ProcessToMove);
      }
      if (ProcessToMove->ProcessID == IDFirstProcess){
	break;
      }
      ProcessToMove =DequeueProcess(WAITINGQUEUE);
    } // while (ProcessToMove)
  } // if (ProcessToMove)
}

/***********************************************************************\
 * Input : whichPolicy (1:FCFS, 2: SRTF, and 3:RR)                      *
 * Output: None                                                         *
 * Function: Selects Process from Ready Queue and Puts it on Running Q. *
\***********************************************************************/
void CPUScheduler(Identifier whichPolicy) {
  ProcessControlBlock *selectedProcess;
  switch(whichPolicy){
    case FCFS : selectedProcess = FCFS_Scheduler();
      break;
    case SRTF : selectedProcess = SRTF_Scheduler();
      break;
    case RR   : selectedProcess = RR_Scheduler();
  }
  if (selectedProcess) {
    selectedProcess->state = RUNNING; // Process state becomes Running
    //selectedProcess->TimeInReadyQueue = Now() - selectedProcess->JobStartTime;
    EnqueueProcess(RUNNINGQUEUE, selectedProcess); // Put process in Running Queue
  }
}

/***********************************************************************\
 * Input : None                                                         *
 * Output: Pointer to the process based on First Come First Serve (FCFS)*
 * Function: Returns process control block based on FCFS                *
 \***********************************************************************/
ProcessControlBlock *FCFS_Scheduler() {
  /* Select Process based on FCFS */
  // Implement code for FCFS
  ProcessControlBlock *selectedProcess = (ProcessControlBlock *) DequeueProcess(READYQUEUE);
  //TimePeriod j = Now();
  return(selectedProcess);
}



/***********************************************************************\
 * Input : None                                                         *
 * Output: Pointer to the process with shortest remaining time (SRTF)   *
 * Function: Returns process control block with SRTF                    *
\***********************************************************************/
ProcessControlBlock *SRTF_Scheduler() {
  /* Select Process with Shortest Remaining Time*/
  // Implement code for SRTF_Scheduler
  //printf("Starting SRTF_Scheduler");
  ProcessControlBlock *currentProcess = (ProcessControlBlock *) DequeueProcess(READYQUEUE);
  ProcessControlBlock *chosenProcess = DequeueProcess(READYQUEUE);
  ProcessControlBlock *originalProcess = currentProcess;    //add original process to use later for a check to see if loop is done
  while(currentProcess){
    if(!chosenProcess){ //if there is not a second process on the queue set current to chosen and return
      chosenProcess == currentProcess;
      //return(chosenProcess);
      break;
    }
    if(originalProcess == currentProcess){   //add in check to make sure to not endlessly loop
      EnqueueProcess(READYQUEUE, currentProcess);
      break;
      //return chosenProcess;
    }
    else if(currentProcess->RemainingCpuBurstTime < chosenProcess->RemainingCpuBurstTime){ //compare current process with relative min time process
      EnqueueProcess(READYQUEUE, chosenProcess);  //makes sure to put process back on queue if its no long min
      chosenProcess = currentProcess;
    }
    else{       //make sure to put currentProcess back on ready queue if not picked
      EnqueueProcess(READYQUEUE, currentProcess);
    }
    currentProcess = DequeueProcess(READYQUEUE);
  }
  return(chosenProcess);
}


/***********************************************************************\
 * Input : None                                                         *
 * Output: Pointer to the process based on Round Robin (RR)             *
 * Function: Returns process control block based on RR                  *
 \***********************************************************************/
ProcessControlBlock *RR_Scheduler() {
  /* Select Process based on RR*/
  ProcessControlBlock *selectedProcess = (ProcessControlBlock *) DequeueProcess(READYQUEUE);
  return(selectedProcess);
}

/***********************************************************************\
 * Input : None                                                         *
 * Output: None                                                         *
 * Function:                                                            *
 *  1)If process in Running Queue needs computation, put it on CPU      *
 *              else move process from running queue to Exit Queue      *
\***********************************************************************/
void Dispatcher() {
  TimePeriod CpuBurstTime;
  ProcessControlBlock *runningProcess = DequeueProcess(RUNNINGQUEUE);
  if (runningProcess == NULL) {
    //printf("%s\n", "null");
    return;}

  if(runningProcess->TimeInCpu == 0) { // if first time this process gets the CPU
    //printf("%s\n", "first time" );
    runningProcess->StartCpuTime = Now(); //Update for this process the field StartCpuTime (in the PCB)
  }

  if (runningProcess->TimeInCpu >= runningProcess->TotalJobDuration) { //if process on running queue is done
    //printf("%s\n", "yay");
    runningProcess->JobExitTime = Now();
    SumMetrics[WT] += runningProcess->TimeInReadyQueue;
    EnqueueProcess(EXITQUEUE,runningProcess); //then add to exit queue
    //int numJobs = NumberofJobs[THGT];
    NumberofJobs[THGT] += 1;

  } else { // Process still needs computing

   //printf("%s\n","it happens" );
   //Determine CpuBurstTime: the length of the CPU burst needed by the process (depends on whether RR or no)
   if(PolicyNumber == RR){
     //runningProcess->RemainingCpuBurstTime = 0.0;      //Update the field RemainingCpuBurstTime in the PCB
   }
   else //Not Round Robin
   {
     if(runningProcess->TotalJobDuration - runningProcess->TimeInCpu < runningProcess->CpuBurstTime){ //if cpu burst is greater than remaining time set that instead
       CpuBurstTime = runningProcess->TotalJobDuration - runningProcess->TimeInCpu;
     }
     else{
       CpuBurstTime = runningProcess->CpuBurstTime;
     }
   }
     OnCPU(runningProcess, CpuBurstTime);             //Put the process on the CPU for CpuBurstTime using the function OnCPU(processOnCPU, CpuBurstTime)
                                                      //where procesOnCPU is a pointer to the process running

    //Update the field TimeInCpu
    runningProcess->TimeInCpu += CpuBurstTime; // SB_ 6/4 use CpuBurstTime instead of PCB-> CpuBurst Time
    EnqueueProcess(RUNNINGQUEUE, runningProcess);
    SumMetrics[CBT] += CpuBurstTime;

  }
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This routine is run when a job is added to the Job Queue    *
\***********************************************************************/
void NewJobIn(ProcessControlBlock whichProcess){
  ProcessControlBlock *NewProcess;
  /* Add Job to the Job Queue */
  NewProcess = (ProcessControlBlock *) malloc(sizeof(ProcessControlBlock));
  memcpy(NewProcess,&whichProcess,sizeof(whichProcess));
  NewProcess->TimeInCpu = 0; // Fixes TUX error
  NewProcess->RemainingCpuBurstTime = NewProcess->CpuBurstTime; // SB_ 6/4 Fixes RR
  EnqueueProcess(JOBQUEUE,NewProcess);
  DisplayQueue("Job Queue in NewJobIn",JOBQUEUE);
  LongtermScheduler(); /* Job Admission  */
}


/***********************************************************************\
* Input : None                                                         *
* Output: None                                                         *
* Function:                                                            *
* 1) BookKeeping is called automatically when 250 arrived              *
* 2) Computes and display metrics: average turnaround  time, throughput*
*     average response time, average waiting time in ready queue,      *
*     and CPU Utilization                                              *
\***********************************************************************/
void BookKeeping(void){
  double end = Now(); // Total time for all processes to arrive
  Metric m;

  // Compute averages and final results
  // ........

  printf("\n********* Processes Managemenent Numbers ******************************\n");
  printf("Policy Number = %d, Quantum = %.6f   Show = %d\n", PolicyNumber, Quantum, Show);
  printf("Number of Completed Processes = %d\n", NumberofJobs[THGT]);
  printf("ATAT=%f   ART=%f  CBT = %f  T=%f AWT=%f\n",
	 SumMetrics[TAT], SumMetrics[RT], SumMetrics[CBT]/end,
	 NumberofJobs[THGT]/Now(), SumMetrics[WT]/NumberofJobs[THGT]);

  exit(0);
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: Decides which processes should be admitted in Ready Queue   *
*           If enough memory and within multiprogramming limit,         *
*           then move Process from Job Queue to Ready Queue             *
\***********************************************************************/
void LongtermScheduler(void){
  ProcessControlBlock *currentProcess = DequeueProcess(JOBQUEUE);
  while (currentProcess) {
    currentProcess->TimeInJobQueue = Now() - currentProcess->JobArrivalTime; // Set TimeInJobQueue
    currentProcess->JobStartTime = Now(); // Set JobStartTime
    EnqueueProcess(READYQUEUE,currentProcess); // Place process in Ready Queue
    currentProcess->state = READY; // Update process state
    currentProcess = DequeueProcess(JOBQUEUE);
  }
}


/***********************************************************************\
* Input : None                                                          *
* Output: TRUE if Intialization successful                              *
\***********************************************************************/
Flag ManagementInitialization(void){
  Metric m;
  for (m = TAT; m < MAXMETRICS; m++){
     NumberofJobs[m] = 0;
     SumMetrics[m]   = 0.0;
  }
  return TRUE;
}

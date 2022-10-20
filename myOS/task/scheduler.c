#include "../include/task.h"
#include "../include/scheduler.h"
#include "../include/myPrintk.h"


scheduler scheduleInUse;
myTCB *arrivedQueue = NULL;
myTCB *readyQueue = NULL;

extern int hh,mm,ss; 
void schedule()
{
	unsigned long **prevTskStkAddr = (unsigned long**)&(readyQueue->stackTop);  
	unsigned long *nextTskStk;
	unsigned long idleFlag = 0;
	readyQueue->status = 4;

	myTCB *choosed, *pre, *idle; 

	if ((choosed = nextTsk()) != NULL)
	{
		myPrintk(0X3,"****************************\n");
		myPrintk(0X3,"* switch to task with tid: %u\n",choosed->tid);
		myPrintk(0x3,"* \n");
	    myPrintk(0x3,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
		myPrintk(0X3,"****************************\n\n");
        nextTskStk = choosed->stackTop;
		choosed->status = 3;
	    
        if (readyQueue->infor.priority == 99)
		{
			idleFlag = 1;
			idle = readyQueue;
		}
		
		 
        pre = readyQueue;
		if ( (pre->next == choosed) && (pre->next->next == NULL) )
		{
			readyQueue = choosed;
		}
		else
		{
			if (pre->next == choosed)
			{
                readyQueue = choosed;
			}
			else
			{
				do
				{
					pre = pre->next;
				} while (pre->next != choosed);

				pre->next = choosed->next;
				choosed->next = readyQueue->next;
				readyQueue = choosed;
			} 
		}

		if (idleFlag == 1)
		{
			choosed = readyQueue;
			while (choosed->next != NULL)
			{
				choosed = choosed->next;
			}
			choosed->next = idle;
			idle->next = NULL;
		}
		
	    context_switch(prevTskStkAddr, nextTskStk);
	    return;
	}
	else
	{   
		// NEVER REACH HERE, there is no task in readyQueue
        myPrintk(0x3,"error raised by schedule\n");
	}
}


myTCB *nextTsk(void){
	return scheduleInUse.nextTsk_func();
}


void enqueueTsk(myTCB *tsk){
    scheduleInUse.enqueueTsk_func(tsk);
}


void dequeueTsk(){
	scheduleInUse.dequeueTsk_func();
}


void schedulerInit(){
    scheduleInUse.schedulerInit_func();
}


void createTsk_hook(myTCB *created){
    scheduleInUse.createTsk_hook(created);
}


void scheduler_tick(void){
    scheduleInUse.tick_hook();	
}





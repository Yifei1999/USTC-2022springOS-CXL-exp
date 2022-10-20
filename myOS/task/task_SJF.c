#include "../include/scheduler.h"
#include "../include/task.h"


extern scheduler task_SJF;


myTCB* nextTsk_SJF(void)
{

    unsigned long curRunTime = readyQueue->next->infor.exeTime;
    myTCB *curSearch = readyQueue->next, *curTsk = readyQueue->next;
    
    while (curSearch->next != NULL )
    {
        curSearch = curSearch->next;
        if ( (curSearch->infor.exeTime) < curRunTime)
        {
            curRunTime = curSearch->infor.exeTime;
            curTsk = curSearch;
        }
    }
    return curTsk;
}


void enqueueTsk_SJF(myTCB *tsk)
{

    myTCB *cur = arrivedQueue;
    if (arrivedQueue == NULL)
    {
        arrivedQueue = tsk;
        tsk->nextarr = NULL;
    }
    else
    {
        if (cur->infor.arrTime > tsk->infor.arrTime)
        {
            tsk->nextarr = cur;
            arrivedQueue = tsk;  
        }
        else
        {    
            while ( cur->nextarr != NULL && cur->nextarr->infor.arrTime <= tsk->infor.arrTime)
            {
                cur = cur->nextarr;
            }
            tsk->nextarr = cur->nextarr;
            cur->nextarr = tsk;
        }
    }
}

void dequeueTsk_SJF()
{
    if (arrivedQueue != NULL)
    {
        arrivedQueue = arrivedQueue->nextarr;
    }
}

extern void (*tick_hook)(void); 
void schedulerInit_SJF(void)
{
    tick_hook = task_SJF.tick_hook;
}

	
void createTsk_hook_SJF(myTCB* created)
{

}


extern int hh, mm, ss;
extern int tick_number;   // 100HZ
void tick_hook_SJF(void)
{
    
    while ( (arrivedQueue != NULL) && ( (arrivedQueue->infor).arrTime <= tick_number/100) )
    {
        tskStart(arrivedQueue);  // a task is coming, fetch a task from ArriveQueue
        dequeueTsk_SJF();
    }
     
    //  1. the readyQueue is not empty && the current task's needed execution time is MORE than the time needed by the next task 
    if ( readyQueue->next != NULL && readyQueue->infor.exeTime > nextTsk_SJF()->infor.exeTime ) // preemptive task method
    {
     
        myTCB *saved = readyQueue;
        
    
        // schedule
        unsigned long **prevTskStkAddr = (unsigned long**)&(readyQueue->stackTop);  
	    unsigned long *nextTskStk;
  
	    myTCB *choosed, *pre; 

	    if ((choosed = nextTsk()) != NULL)
	    {
            
            nextTskStk = choosed->stackTop;
            
            if (choosed->tid == 1)
            {
                myPrintk(0X3,"****************************\n");
                myPrintk(0X3,"Now in idle, waiting for a task to come...\n");
                myPrintk(0x5,"* \n");
	            myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
                myPrintk(0X3,"****************************\n\n");
            }
            else
            {
                myPrintk(0X3,"****************************\n");
		        myPrintk(0X3,"* switch to task with tid: %u\n",choosed->tid);
                myPrintk(0x5,"* \n");
	            myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
		        myPrintk(0X3,"****************************\n\n");
            }
            

            nextTskStk = choosed->stackTop;
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
                    } 
                    while (pre->next != choosed);
                    
                    pre->next = choosed->next;
                    choosed->next = readyQueue->next;
                    readyQueue = choosed;
                }
                
            }
                        
            myTCB *cur = readyQueue;
            while ( cur->next != NULL )
            {
                cur = cur->next;
            }

            cur->next = saved;    // move the unfinished job to the end of queue
            saved->next = NULL;



            context_switch(prevTskStkAddr, nextTskStk);
            return;
        }
    }


}




scheduler task_SJF = {2,
            nextTsk_SJF,
            enqueueTsk_SJF,
            dequeueTsk_SJF,
            schedulerInit_SJF,
            createTsk_hook_SJF,
            tick_hook_SJF
            };
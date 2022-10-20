#include "../include/scheduler.h"
#include "../include/task.h"



extern scheduler task_PF;


myTCB* nextTsk_PF(void)
{
    unsigned long curPriority = readyQueue->next->infor.priority;
    myTCB *curSearch = readyQueue->next, *curTsk = readyQueue->next;
    
    while (curSearch->next != NULL )
    {
        curSearch = curSearch->next;
        if ( (curSearch->infor.priority) < curPriority)
        {
            curPriority = curSearch->infor.priority;
            curTsk = curSearch;
        }
    }
    return curTsk;
}


void enqueueTsk_PF(myTCB *tsk)
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

void dequeueTsk_PF()
{
    if (arrivedQueue != NULL)
    {
        arrivedQueue = arrivedQueue->nextarr;
    }
}

extern void (*tick_hook)(void); 
void schedulerInit_PF(void)
{
    tick_hook = task_PF.tick_hook;
}

	
void createTsk_hook_PF(myTCB* created)
{

}


extern int tick_number;   // 100HZ
void tick_hook_PF(void)
{
    
    while ( (arrivedQueue != NULL) && ( (arrivedQueue->infor).arrTime <= tick_number/100) )
    {
        
        tskStart(arrivedQueue);
        dequeueTsk_PF();
    }
}



scheduler task_PF = {1,
            nextTsk_PF,
            enqueueTsk_PF,
            dequeueTsk_PF,
            schedulerInit_PF,
            createTsk_hook_PF,
            tick_hook_PF
            };
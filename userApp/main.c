#include "../myOS/userInterface.h"   //interface from kernel

#include "shell.h"
#include "memTestCase.h"

void wallClock_hook_main(void){
    int _h, _m, _s;
    char hhmmss[]="hh:mm:ss\0\0\0\0";

    getWallClock(&_h,&_m,&_s);
    sprintf(hhmmss,"%02d:%02d:%02d",_h,_m,_s);
    put_chars(hhmmss,0x7E,24,72);
}

void doSomeTestBefore(void){		
    setWallClock(00,00,00);		//set time 18:59:59
        setWallClockHook(&wallClock_hook_main);
}

void printlist(void)
{
    myTCB *cur = readyQueue;
    myPrintk(0x5,"\n****** print ready list *******\n*\n");
    while (cur != NULL)
    {
        myPrintk(0x5,"* - pid: %u, prior: %u, exetime: %u\n",cur->tid,cur->infor.priority,cur->infor.exeTime);
        cur = cur->next;
    }
    

    cur = arrivedQueue;
    myPrintk(0x5,"*\n*      print unarrived list   \n*\n");
    while (cur != NULL)
    {
        myPrintk(0x5,"* - pid: %u, prior: %u, exetime: %u\n",cur->tid,cur->infor.priority,cur->infor.exeTime);
        cur = cur->nextarr;
    }
    
    myPrintk(0x5,"*\n*************** end ***************\n\n");
}

void myMain(void)
{    	
    clear_screen();

    myPrintk(0x5,"\n****** enter my main *******\n\n");
    doSomeTestBefore();
    initShell();
    myPrintk(0x2,"create tasks...\n");
    
    
    schedulerInit(); 
    {
     
        unsigned long tmp_id;
        tskPara scheduleInfor;
        
        // idle task (tid:1)
        initTskPara(&scheduleInfor);
		setTskPara(PRIORITY, 99, &scheduleInfor);
		setTskPara(ARRTIME, 0, &scheduleInfor);
        setTskPara(EXETIME, 0xffffffff, &scheduleInfor);
        unsigned long tid = createTsk(taskIdleBody, &scheduleInfor);
        dequeueTsk();
        tskStart(&taskPool[tid]);

        // task1 (tid:2)
        initTskPara(&scheduleInfor);
        setTskPara(PRIORITY, 1, &scheduleInfor);
		setTskPara(ARRTIME, 21, &scheduleInfor);
        setTskPara(EXETIME, 2, &scheduleInfor);
        tmp_id = createTsk(task1Body, &scheduleInfor);
        
        // task2 (tid:3)
        initTskPara(&scheduleInfor);
        setTskPara(PRIORITY, 5, &scheduleInfor);
		setTskPara(ARRTIME, 0, &scheduleInfor);
        setTskPara(EXETIME, 8, &scheduleInfor);
        tmp_id = createTsk(task2Body, &scheduleInfor);
        
        // task3 (tid:4)
        initTskPara(&scheduleInfor);
        setTskPara(PRIORITY, 4, &scheduleInfor);
		setTskPara(ARRTIME, 3, &scheduleInfor);
        setTskPara(EXETIME, 2, &scheduleInfor);
        tmp_id = createTsk(task3Body, &scheduleInfor);
        
        // task4 (tid:5)
        initTskPara(&scheduleInfor);
        setTskPara(PRIORITY, 3, &scheduleInfor);
		setTskPara(ARRTIME, 6, &scheduleInfor);
        setTskPara(EXETIME, 6, &scheduleInfor);
        tmp_id = createTsk(task4Body, &scheduleInfor);
        
        // task5 (tid:4)
        initTskPara(&scheduleInfor);
        setTskPara(PRIORITY, 2, &scheduleInfor);
		setTskPara(ARRTIME, 6, &scheduleInfor);
        setTskPara(EXETIME, 3, &scheduleInfor);
        tmp_id = createTsk(task5Body, &scheduleInfor);

    }

    myPrintk(0x2,"\n");
    myPrintk(0x2,"ready to start the tasks!\n\n");

    printlist();

    tskEnd();
    return;
}


#include "include/i8253.h"
#include "include/i8259.h"
#include "include/irq.h"
#include "include/uart.h"
#include "include/vga.h"
#include "include/mem.h"
#include "include/myPrintk.h"
#include "include/task.h"
#include "../myOS/userInterface.h"


extern void myMain(void);		//TODO: to be generalized

void pressAnyKeyToStart(void){
	myPrintk(0x5,"Prepare uart device\n");
	myPrintk(0x5,"Then, press any key to start ...\n");

	uart_get_char();
}

void osStart(void){
	pressAnyKeyToStart(); // prepare for uart device

    myPrintk(0x2,"set the strategy for task schedule:\n");
	myPrintk(0x2," - 1. FIFO\n");
	myPrintk(0x2," - 2. PF\n");
	myPrintk(0x2," - 3. SJF (preemptive)\n");
	myPrintk(0x2,"type 1/2/3 to select\n");
    char task_stag = uart_get_char();  // set the strategy for task schedule
	clear_screen();

	init8259A();
	init8253();
	enable_interrupt();

	clear_screen();
     
	extern scheduler task_FIFO;
    extern scheduler task_PF;
	extern scheduler task_SJF;
	switch (task_stag)
	{
		case '1': 
			scheduleInUse = task_FIFO;
			break;
		
		case '2': 
			scheduleInUse = task_PF;
			break;
		
		case '3': 
			scheduleInUse = task_SJF;
			break;
		
		default:
		    scheduleInUse = task_FIFO;
			break;
	}
     
	pMemInit();  //after this, we can use kmalloc/kfree and malloc/free

	// finished kernel init
	// NOW, run userApp
	myPrintk(0x2,"START RUNNING......\n");	
	{
		tskPara scheduleInfor;
		initTskPara(&scheduleInfor);
		setTskPara(PRIORITY, 0, &scheduleInfor);
		setTskPara(ARRTIME, 0xffffffff, &scheduleInfor);
        setTskPara(EXETIME, 0, &scheduleInfor);
		
		taskPool[taskNum].infor = scheduleInfor;
		taskPool[taskNum].status = 1;
	    taskPool[taskNum].tid = taskNum;
	    taskPool[taskNum].taskBody = myMain;
		taskNum ++;
		unsigned long tid = taskPool[taskNum-1].tid;
	
	    tskStart(&taskPool[tid]);
	}
	
	startMultitask(); 

	myPrintk(0x2, "STOP RUNNING......ShutDown\n");
	while(1);
}



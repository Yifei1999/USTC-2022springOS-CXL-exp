#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;//可用的内存的起始地址
unsigned long pMemSize;//可用的大小
typedef unsigned short int doublebyte;// 双字，用于写入0xAA55进行探测

unsigned long pMemHandler;
unsigned long pMemHandler_kernel;

void memTest(unsigned long start, unsigned long grainSize){
	/*本函数需要实现！！！*/
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从某一个地址开始的内存是否可用，具体算法就可以用ppt上说的写了读看是否一致。
	注意点两个：
	1、开始的地址要大于1M，需要做一个if判断。
	2、grainsize不能太小，也要做一个if判断
	*/
	if(start <= 0x100000)
	{
		myPrintk(0x4,"raise warning: memory access below 1M, change boundary to 1M!\n");
		start = 0x100000;
	}
	if(grainSize <= 0x400)  // 0x400 = 1K 最小为1K
	{
		myPrintk(0x4,"raise warning: memory detect. footstep is too small, footstep: %x\n, changed to 1K",grainSize);
		grainSize = 0x400;
	}

	
	doublebyte *ptr;    // the write pointer
	doublebyte temp;    // make a copy and recovery
	unsigned long address;    // the start address of a grain 

	for(address=start; ; address+=grainSize)
	{
        ptr = (doublebyte*)address; //需要测试的地址起始
        temp = *ptr;
		
		*ptr = 0xAA55;
        if(*ptr != 0xAA55)    break;
		*ptr = 0x55AA;
        if(*ptr != 0x55AA)   break;
		*ptr = temp;
	 
		ptr = (doublebyte*)(address + grainSize - 2);
		temp = *ptr;
		*ptr = 0xAA55;
        if(*ptr != 0xAA55)   break;
		*ptr = 0x55AA;
        if(*ptr != 0x55AA) 	break;
		
		*ptr = temp;
	}

    pMemStart = start;
	pMemSize = address - start;

	myPrintk(0x7,"MemStart: %x  \n",pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n",pMemSize);
	

	return;
}

extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	myPrintk(0x7,"-----------------------\n");
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}

    myPrintk(0x7,"pMemStart: %x\n", pMemStart);
	myPrintk(0x7,"pMemSize: %x\n", pMemSize);
	pMemHandler_kernel = dPartitionInit(pMemStart,0x1000000-pMemStart);	  // 内核分配区，从操作系统结束处到0x1000000
	pMemHandler = dPartitionInit(0x1000000,pMemStart+pMemSize-0x1000000);	// 用户分配区，从0x1000000开始到最大范围

	return;
}

#include "../../include/myPrintk.h"
// 一个EEB表示一个空闲可用的Block
typedef struct EEB {
	unsigned long next_start;
}EEB;	//占4个字节

#define EEB_size 4

void showEEB(struct EEB *eeb){
	myPrintk(0x7,"EEB(start=0x%x, next=0x%x)\n", eeb, eeb->next_start);
}

//eFPartition是表示整个内存的数据结构
typedef struct eFPartition{
	unsigned long totalN;
	unsigned long perSize;  //unit: byte	
	unsigned long firstFree;
}eFPartition;	//占12个字节

#define eFPartition_size 12

void showeFPartition(struct eFPartition *efp){
	myPrintk(0x5,"eFPartition(start=0x%x, totalN=0x%x, perSize=0x%x, firstFree=0x%x)\n", efp, efp->totalN, efp->perSize, efp->firstFree);
}

void eFPartitionWalkByAddr(unsigned long efpHandler){
	//本函数需要实现！！！
	/*功能：本函数是为了方便查看和调试的。
	1、打印eFPartiiton结构体的信息，可以调用上面的showeFPartition函数。
	2、遍历每一个EEB，打印出他们的地址以及下一个EEB的地址（可以调用上面的函数showEEB）
	*/
    
	showeFPartition( (eFPartition *)efpHandler );
	unsigned long curEEBaddress =  ((eFPartition *)efpHandler)->firstFree;
	
	while ( curEEBaddress != 0)
	{
		showEEB( (EEB*)curEEBaddress );
		curEEBaddress = ((EEB*)curEEBaddress)->next_start;
	}
    
    return;
}


unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n){
	//本函数需要实现！！！
	/*功能：计算占用空间的实际大小，并将这个结果返回
	1.根据参数persize（每个大小）和n个数计算总大小，注意persize的对齐。
		例如persize是31字节，你想8字节对齐，那么计算大小实际代入的一个块的大小就是32字节。
	2.同时还需要注意“隔离带”EEB的存在也会占用4字节的空间。
		typedef struct EEB {
			unsigned long next_start;
		}EEB;	
	3.最后别忘记加上eFPartition这个数据结构的大小，因为它也占一定的空间。

	*/

	unsigned long memorySize = 0x0;
    
	memorySize += eFPartition_size;
	memorySize += perSize*n;
	return memorySize;

}

unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n){
	//本函数需要实现！！！
	/*功能：初始化内存
	1.需要创建一个eFPartition结构体，需要注意的是结构体的perSize不是直接传入的参数perSize，需要对齐。结构体的next_start也需要考虑一下其本身的大小。
	2.就是先把首地址start开始的一部分空间作为存储eFPartition类型的空间
	3.然后再对除去eFPartition存储空间后的剩余空间开辟若干连续的空闲内存块，将他们连起来构成一个链。注意最后一块的EEB的nextstart应该是0
	4.需要返回一个句柄，也即返回eFPartition *类型的数据
	注意的地方：
		1.EEB类型的数据的存在本身就占用了一定的空间。
	*/

    // 用户给定起始的地址 start, 每个用户块的大小 perSize（包括起始部分）, 块的总个数 n 
    
	unsigned long nextPartitionAdd = start + eFPartition_size;
    ((eFPartition*)start)->firstFree = nextPartitionAdd;
	((eFPartition*)start)->perSize = perSize;
	((eFPartition*)start)->totalN = n;
     
    while ( n != 0 )
	{
        ((EEB*)nextPartitionAdd)->next_start = nextPartitionAdd + perSize;
		nextPartitionAdd =  nextPartitionAdd + perSize;
		n--;
	}
	((EEB*)(nextPartitionAdd - perSize))->next_start = 0x0;
    
    myPrintk(0x7,"set up partition, start at: %x,persize: %x, n: %x\n",start , perSize, n);
	myPrintk(0x7,"--------------------------------------------------------\n");
	return (eFPartition*)start;
}


unsigned long eFPartitionAlloc(unsigned long EFPHandler){
	//本函数需要实现！！！
	/*功能：分配一个空间
	1.本函数分配一个空闲块的内存并返回相应的地址，EFPHandler表示整个内存的首地址
	2.事实上EFPHandler就是我们的句柄，EFPHandler作为eFPartition *类型的数据，其存放了我们需要的firstFree数据信息
	3.从空闲内存块组成的链表中拿出一块供我们来分配空间，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EEB类型的数据的存在本身就占用了一定的空间。

	*/
    unsigned long allocAddress;

    if ( ((eFPartition*)EFPHandler)->firstFree != 0x0 )
	{
		allocAddress =  ((eFPartition*)EFPHandler)->firstFree;
		((eFPartition*)EFPHandler)->firstFree = ((EEB*)allocAddress)->next_start;
		return allocAddress;
	}
	else
	{
		myPrintk(0x7,"error raised by eFPartitionAlloc: not spare partition!\n");
		return 0;
	}
}


unsigned long eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart){
	//本函数需要实现！！！
	/*功能：释放一个空间
	1.mbstart将成为第一个空闲块，EFPHandler的firstFree属性也需要相应大的更新。
	2.同时我们也需要更新维护空闲内存块组成的链表。
	
	*/
    
	((EEB*)mbStart)->next_start = ((eFPartition*)EFPHandler)->firstFree;
	((eFPartition*)EFPHandler)->firstFree = mbStart;
    
	return 0;
}

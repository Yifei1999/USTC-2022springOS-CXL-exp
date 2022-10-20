#include "../../include/myPrintk.h"

#define dPartition_size ((unsigned long)0x8) // 句柄大小
#define EMB_size ((unsigned long)0x8)  // 空闲BLOCK大小
#define rarememoryPTR ((unsigned long)0xFFFFFFFF)  //用于填写EMB的nextStart，表示已经到头
#define minMemoryAllo 0x4   // 内存分配最小大小为 4bytes

//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

// EMB每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, blongs to user
	};	                           
} EMB;	//共占8个字节

void showdPartition(struct dPartition *dp){   // dp = dynamic partition
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}

void showEMB(struct EMB * emb){  // EMB = empty memory block
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){  
	// 在start 处，在 totalSize 长度的范围内建立分区
	// start 是内存区起始地址（即DP区的起始地址）
	// totalsize 是分配的全部空间，包括了DP和EMB全部内容的大小

	//本函数需要实现！！！
	/*功能：初始化内存。
	1.在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2.然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3.返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	
	*/
    dPartition meRecord;
	EMB newPartition;
    dPartition *handlePtr = (dPartition*)start;   // DP的起始地址
	EMB *partitionPtr = (EMB*)(start + dPartition_size);  // EMB的起始地址

	meRecord.size = totalSize - dPartition_size;
	meRecord.firstFreeStart = start + dPartition_size;

	newPartition.size = totalSize - dPartition_size - EMB_size  + 4;  // total - 8 - 8 + 4
	newPartition.nextStart = rarememoryPTR;
    
	*handlePtr = meRecord;
	*partitionPtr = newPartition;
	return start;
}

void dPartitionWalkByAddr(unsigned long dp){
	//本函数需要实现！！！
	/*功能：本函数遍历输出EMB 方便调试
	1.先打印dP的信息，可调用上面的showdPartition。
	2.然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息
	*/
    dPartition *handlePtr = (dPartition*)dp;

	myPrintk(0x3, "handle:\n start:%x\n size of whole memory(without DP):%x\n first EMB address:%x\n", dp, handlePtr->size, handlePtr->firstFreeStart);

	unsigned long curEMBadd = handlePtr->firstFreeStart;
	EMB *partitionPtr = (EMB*)(handlePtr->firstFreeStart);
	for( ; ; )
	{
		showEMB(partitionPtr);

        // traversal to next EMB 
        if (partitionPtr->nextStart == rarememoryPTR)  // reach the end
		{
			break;
		}
		else
		{
            curEMBadd = partitionPtr->nextStart;
		    partitionPtr = (EMB *)curEMBadd;
		}
	}
	myPrintk(0x3, "\n");
    return;
}

//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	//本函数需要实现！！！
	/*功能：分配一个空间
	1.使用firstfit的算法分配空间，当然也可以使用其他fit，不限制。
	2.成功分配返回首地址，不成功返回0
	3.从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。

	*/
    if (size <= minMemoryAllo)
	{
		size = minMemoryAllo;
	}

	EMB *partitionPtr;
	unsigned long curEMBadd = ((dPartition*)dp)->firstFreeStart;   // 当前检查的块的地址
	unsigned long preEMBadd = dp;    // 上一个检查的块的地址
	unsigned short int flag = 1;
	
	do
	{
		partitionPtr = (EMB*)curEMBadd;

		// check the partition
		if(partitionPtr->size <= size)
		{
			// jump to next partition
			preEMBadd = curEMBadd;
			curEMBadd = partitionPtr->nextStart;
			flag = 0; 
		}
		else
		{
            unsigned long size_temp = partitionPtr -> size;
            unsigned long nextStart_temp = partitionPtr -> nextStart;

            partitionPtr->size = size;
            unsigned long return_address = curEMBadd + 4;
            // myPrintk(0x3, "starting address allocated to user: %x\n", return_address);

            curEMBadd += size + (EMB_size - 4);


            // 处理前项
            if (flag == 0){
                ((EMB*)preEMBadd) -> nextStart = curEMBadd; 
            }
            else{
                ((dPartition*)preEMBadd)->firstFreeStart = curEMBadd;
            }
            // 为剩余空间重建数据结构
            partitionPtr = (EMB*)curEMBadd;

            partitionPtr -> size = size_temp - (size + (EMB_size - 4) );
            partitionPtr -> nextStart = nextStart_temp;
            
            // myPrintk(0x3, "-----------------------\n");
            return return_address;    // 有一个 4bytes 的头
		}
		
	}
	while (curEMBadd != rarememoryPTR);
    
	myPrintk(0x3, "error raised by dPartitionAllocFirstFit: not enough memory!\n");
	return 0;
}

unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	//本函数需要实现！！！
	/*功能：释放一个空间
	1.按照对应的fit的算法释放空间
	2.注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3.需要考虑两个空闲且相邻的内存块的合并
	*/
    
	unsigned long targetEMBaddress = start - 0x4;   // 需要删除的EMB地址
	unsigned long preAddressPOS, curAddressPOS;  // 将要删除的块的前驱和后继EMB的地址
	unsigned short int flaglf = 0, flagrt = 0, flagstart = 0;   
	// flaglf：回收的块需要与左边紧邻空闲块合并
	// flagrt：回收的块需要与右边紧邻空闲块合并
	// flagstart: 回收的块将是句柄区后的第一个EMB
    
	if ( !(dp <= targetEMBaddress) )
	{
		myPrintk(0x3, "error1 raised by dPartitionFreeFirstFit: value \"start\" is not valued!\n");
		return 0;
	}
	if ( !( ( ((EMB*)targetEMBaddress)->size + targetEMBaddress) <= (dp + ((dPartition*)dp)->size)) )
	{
		myPrintk(0x3, "error raised by dPartitionFreeFirstFit: value \"start\" is not valued!\n");
		return 0;
	}

	preAddressPOS = dp;
	curAddressPOS = ((dPartition*)dp) -> firstFreeStart;

	while( !( preAddressPOS <= start && start <= curAddressPOS ) )  // 判断释放块的位置
	{
		flagstart = 1;
		preAddressPOS = curAddressPOS;
		curAddressPOS = ((EMB*)curAddressPOS)->nextStart;
	}

    if (flagstart == 0) // 释放块的位置在句柄开始处
	{
        if (preAddressPOS + dPartition_size ==  targetEMBaddress)
		{
			flaglf = 1;
        }
	}
	else 
	{
		if ( (preAddressPOS + 0x4 + ((EMB*)preAddressPOS)->size) == targetEMBaddress)
		{
			flaglf = 1;
		}
	}	

	if ( (targetEMBaddress + 0x4 + ((EMB*)targetEMBaddress)->size) == curAddressPOS )   // 释放块需要与右边的空闲块合并
	{
		flagrt = 1;
	}
    
    unsigned long int freespacesize = ((EMB*)targetEMBaddress)->size + 0x4;
	if (flaglf == 0)
	{    // 左边没有临近块
	    ((EMB*)preAddressPOS)->nextStart = targetEMBaddress;
	}
	else
	{
        if (flagstart == 0)
        {
            ((dPartition*)preAddressPOS)->firstFreeStart = targetEMBaddress;
            ((EMB*)targetEMBaddress)->nextStart = curAddressPOS;
            ((EMB*)targetEMBaddress)->size = ((EMB*)targetEMBaddress)->size;
        }
        else
        {
            ((EMB*)preAddressPOS)->nextStart = curAddressPOS;
		    ((EMB*)preAddressPOS)->size += freespacesize;
		    targetEMBaddress = preAddressPOS;
        }
	}


	if (flagrt == 0)
	{
        ((EMB*)targetEMBaddress)->nextStart = curAddressPOS;
	}
	else
	{
		((EMB*)targetEMBaddress)->nextStart = ((EMB*)curAddressPOS)->nextStart;
		((EMB*)targetEMBaddress)->size += ((EMB*)curAddressPOS)->size + 0x4;
	}
	
	return 1;
}

//wrap: we select firstFit, you can select another one
//user need not know this
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}

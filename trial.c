#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const int VM_SIZE=256;
const int PAGE_SIZE=256;
const int TLB_SIZE=16;
const int MM_SIZE=256;

//shuffling
void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int main(int argc, char* argv[])
{
	FILE *fd;
	
	if(argc<2)
	{
		printf("NOT ENOUGH AGRUMENTS\n EXITING\n");
		return 0;
	}
	fd=fopen(argv[1],"r");
	if(fd==NULL)
	{
		printf("ERROR OPENING FILE\n FILE FAILED TO OPEN\n");
		return 0;
	}
	char *value=NULL;
	size_t len=0;
	ssize_t read;
	long long page_no,offset,page_table,totalhits=0,fault=0,pages=0;
	
	int qp=0;						//to maintain the queue position
	int physicalad=0,frame,logicalad;
	
	int tlb[TLB_SIZE][2];
	int pagetable[PAGE_SIZE];

	memset(tlb,-1,TLB_SIZE*2*sizeof(tlb[0][0]));
	memset(pagetable,-1,sizeof(pagetable));

    for(int i = 0; i < 256; i++)
    {
        pagetable[i] = i;
    }
    shuffle(pagetable,256);
    for(int i = 0; i < 10; i++)
    {
        printf("%d\n",pagetable[i]);
    }
    printf("*****************\n");
    //memset(pagetable,-1,sizeof(pagetable));
	
	int mask=255,mask1=62580,i,hit;
	
	while((read=getline(&value,&len,fd))!=-1)
	{
		pages++;
		//get page number and offset from logical address
		page_no=atoi(value);
		page_no=page_no>>8;
		page_no=page_no & mask;
		
		offset=atoi(value);
		offset=offset & mask;
		
		logicalad=atoi(value);
		frame=0,physicalad=0;
		
		hit=0;			//1 if found in TLB
		printf("%d is the Logical Address\n",logicalad);
		//CHECK IN TLB
		for(i=0;i<TLB_SIZE;i++)
		{
			if(tlb[i][0]==page_no)
			{
				hit=1;
				totalhits++;
				frame=tlb[i][1];
				break;
			}
		}
        //if present in tlb
        if(hit == 1)
            printf("HIT\n");
        else
        {
            printf("Miss\n");
            fault++;
            frame = pagetable[page_no];
            tlb[qp][0] = page_no;
            tlb[qp][1] = frame;
            qp++;
            qp=qp%16;
        }
		if(logicalad<10000)
		printf("VIRTUAL ADDRESS = %d \t\t\t",logicalad);
		else
		printf("VIRTUAL ADDRESS = %d \t\t",logicalad);
		printf("Page No: %lld and Offset: %lld\n",page_no,offset);
		physicalad=frame*PAGE_SIZE + offset;
		printf("PHYSICAL ADDRESS = %d\n",physicalad);
	}
	double hitrate=(double)totalhits/pages*100;
	double faultrate=(double)fault/pages*100;
	printf("\nTLB HIT RATE= %.2f %c", hitrate,'%');
	printf("\nTLB MISS RATE= %.2f %c",(100-hitrate),'%');
	printf("\nPAGE TABLE HIT RATE= %.2f %c", faultrate,'%');
	printf("\nPAGE TABLE MISS RATE= %.2f %c\n", (100-faultrate),'%');
}
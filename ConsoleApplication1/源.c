#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include "../GabageCollection/GabageCollection.h"

#pragma comment(lib,"GabageCollection.lib")

void **poiner;

void a(GabageCollection *GC)
{
	void *address;
	GC->VirtualTable->Allocate(GC,&address,10);
	void *temp;
	GC->VirtualTable->Copy(GC,&address,&temp);
}

void test(void *Parameter)
{
	GabageCollection *GC=(GabageCollection*)Parameter;
	void *address;
	/*GC->VirtualTable->Copy(GC,poiner,&address);
	*poiner=NULL;
	system("pause");*/
	GC->VirtualTable->Allocate(GC,&address,10);
	void *temp=NULL;
	GC->VirtualTable->Copy(GC,&address,&temp);
	address=NULL;
	temp=NULL;

	a(GC);

	GC->VirtualTable->Allocate(GC,&address,5);
}

int main()
{
	GabageCollection *GC=InitializeGabageCollection();
	GC->ShowErrorMessage=true;
	void *address;
	GC->VirtualTable->Allocate(GC,&address,10);
	void *temp/*=NULL*/;
	GC->VirtualTable->Copy(GC,&address/*NULL*/,&temp);
	address=NULL;
	printf("0x%llX\n",temp);
	//temp=NULL;
	poiner=&temp;
	
	//a(GC);

	//GC->VirtualTable->Allocate(GC,&address,5);
	_beginthread(test,0,GC);
	//GC->VirtualTable->Dispose(GC,true);
	system("pause");
	return 0;
}
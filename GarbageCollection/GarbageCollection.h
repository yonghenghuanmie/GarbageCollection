#pragma once

#include "Macro.h"

#ifdef __cplusplus

typedef struct _GcNode
{
	HANDLE hThread;
	shared_ptr<void> value;
}GcNode;

class GarbageCollection
{
public:
	virtual void Allocate(void **address,int size);
	virtual void Copy(void **source,void **destination);
	virtual void Dispose(bool FreeMemory);
	static void GcThread(GarbageCollection *instance);
	bool ShowErrorMessage=false;
	static GarbageCollection *instance;
private:
	void FreeGcNode(map<void**,GcNode>::iterator &iter);
	void* IsReadable(void **address);
	void IsWriteable(void **address,void *value);
	void Output(){ cout<<endl; }
	template<class T,class ...Args>
	void Output(const T &first,const Args &...others)
	{
		if(ShowErrorMessage)
		{
			cout<<first;
			Output(others...);
		}
	}
	map<void**,GcNode> data;
	atomic_flag flag=ATOMIC_FLAG_INIT;
	volatile bool quit=false;
	volatile bool isquit=false;
#ifdef _DEBUG
	int freecount=0;
#endif // _DEBUG
};

#else

#include <stdbool.h>

typedef struct _VIRTUALTABLE
{
	void (*Allocate)(struct _GarbageCollection *this,void **address,int size);
	void (*Copy)(struct _GarbageCollection *this,void **source,void **destination);
	void (*Dispose)(struct _GarbageCollection *this,bool FreeMemory);
}VIRTUALTABLE;

typedef struct _GarbageCollection
{
	VIRTUALTABLE *VirtualTable;
	bool ShowErrorMessage;
	//...
}GarbageCollection;

#endif // __cplusplus

declaration GarbageCollection * InitializeGarbageCollection();
#include <cassert>
#include <new>
#include <map>
#include <atomic>
#include <memory>
#include <thread>
#include <iostream>
#include <windows.h>

#define Export
using namespace std;
#include "GarbageCollection.h"

GarbageCollection* GarbageCollection::instance=nullptr;

void GarbageCollection::Allocate(void **address,int size)
{
	while(flag.test_and_set());
	try
	{
		data[address]=GcNode
		({
			OpenThread(THREAD_GET_CONTEXT,false,GetCurrentThreadId()),
			shared_ptr<void>
			(new char[size](),
				[this](char *p)
				{
					delete[] p;
				#ifdef _DEBUG
					cout<<"free"<<++freecount<<endl;
				#endif // _DEBUG
				}
			)
		});
		assert(data[address].hThread);
		IsWriteable(address,data[address].value.get());
	}
	catch(const bad_alloc &e)
	{
		Output(__func__,":",e.what());
	}
	catch(const invalid_argument &e)
	{
		auto&& iter = data.find(address);
		FreeGcNode(iter);
		Output(__func__,":",e.what());
	}
	flag.clear();
}

void GarbageCollection::Copy(void **source,void **destination)
{
	while(flag.test_and_set());
	try
	{
		data[destination]=GcNode({OpenThread(THREAD_GET_CONTEXT,false,GetCurrentThreadId()),data.at(source).value});
		assert(data[destination].hThread);
		IsWriteable(destination,data[destination].value.get());
	}
	catch(const out_of_range &e)
	{
		*destination=NULL;
		Output(__func__,":",e.what());
	}
	catch(const invalid_argument &e)
	{
		auto&& iter = data.find(destination);
		FreeGcNode(iter);
		Output(__func__,":",e.what());
	}
	flag.clear();
}

void GarbageCollection::GcThread(GarbageCollection *instance)
{
	CONTEXT Context={0};
	Context.ContextFlags=CONTEXT_ALL;
	do
	{
		while(instance->flag.test_and_set());
		for(auto iter=instance->data.begin();iter!=instance->data.end();)
		{
			try
			{
				if(!GetThreadContext(iter->second.hThread,&Context))
				{
					DWORD ErrorCode=GetLastError();
					if(ErrorCode==0x1F)
					{
						instance->FreeGcNode(iter);
						continue;
					}
					else
						instance->Output("GetThreadContext failed:",ErrorCode);
				}
				else if((unsigned long long)iter->first<Context.Rsp)
				{
					instance->FreeGcNode(iter);
					continue;
				}


				if(instance->IsReadable(iter->first)!=iter->second.value.get())
				{
					instance->FreeGcNode(iter);
					continue;
				}
			}
			catch(const invalid_argument &e)
			{
				instance->FreeGcNode(iter);
				instance->Output(__func__,":",e.what());
				continue;
			}
			++iter;
		}
		instance->flag.clear();
	} while(!instance->quit);
	instance->isquit=true;
}

inline void GarbageCollection::FreeGcNode(map<void**,GcNode>::iterator &iter)
{
	CloseHandle(iter->second.hThread);
	iter=data.erase(iter);
}

void* GarbageCollection::IsReadable(void ** address)
{
	__try
	{
		return *address;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		throw(invalid_argument("This address is not readable!"));
	}
}

void GarbageCollection::IsWriteable(void ** address,void *value)
{
	__try
	{
		*address=value;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		throw(invalid_argument("This address is not writeable!"));
	}
}

void GarbageCollection::Dispose(bool FreeMemory)
{
	quit=true;
	while(isquit==false);
	if(FreeMemory)
		for(auto iter=data.begin();iter!=data.end();)
			FreeGcNode(iter);
	delete this;
}

GarbageCollection * InitializeGarbageCollection()
{
	if(GarbageCollection::instance==nullptr)
	{
		GarbageCollection::instance=new GarbageCollection();
		thread *ThreadObject=new thread(GarbageCollection::GcThread,GarbageCollection::instance);
		ThreadObject->detach();
	}
	return GarbageCollection::instance;
}
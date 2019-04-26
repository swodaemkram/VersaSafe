/*
	Module: queue.cpp
	Author: Gary Conway
	Created: 2-24-2019

	this module imlepements a call queue via a vector<>
	we create an instance of the EventDriver class, and use its methods
	to add, remove and process events


INFO HERE - this is the model I used to dev this queue
https://embeddedartistry.com/blog/2017/1/26/c11-improving-your-callback-game
https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/callbacks.cpp


Usage:

	the following fns are publicly available, and declarations are in queue.h

	void InitEventDriver(void)
	uint32_t AddCallBack(const cb_t &cb)
	void ExecCallBacks(void)
	void RemoveCallBack(uint32_t handle)

	#include "queue.h"
	InitEventDriver();
	uint32_t handle = AddCallBack(&functionname);	// add callback to queue (returns handle)
	ExecCallBacks();			// execute all functions in the queue
	RemoveCallBack(handle);		// remove this call back


	Note: all functions to be stored in queue, must be defined with extern wrapper, as follows

	extern "C"
	{
			void function(void)
			{
				return;
			}
	}

*/

using namespace std;

#include <iostream>
#include <functional>
#include <iterator>
#include <vector>
#include "queue.h"

// declarations
//typedef std::function<void(int)> cb_t; (see queue.h)


struct cb_event_t
    {
    std::function<void(int)> cb;
    uint32_t handle;
    };

std::vector<cb_event_t> callbacks_;









class EventDriver {

private:
#define DEFAULT_HANDLE 1001
uint32_t curhandle=DEFAULT_HANDLE;



public:

    EventDriver(uint32_t val) : val_(val), callbacks_() { }


    // Register a callback, returns the assigned handle
    uint32_t register_callback(const cb_t &cb)
    {
        // add callback to end of callback list
        callbacks_.push_back({cb,curhandle++});
		return curhandle-1;
    }

//  void remove_callback(const cb_t &cb)
	void remove_callback(uint32_t handle)
    {


        vector<cb_event_t>::iterator ptr;
        for (ptr = callbacks_.begin(); ptr < callbacks_.end(); ptr++)
        {
printf("ptrhandle: %d   handle:%d\n",ptr->handle,handle);
            if (ptr->handle == handle)
            {
cout  << ptr->handle << " xx \n";
                 callbacks_.erase(ptr);
            }

        }
    }


    // get the number of elements in the vector
    int size(void)
    {
        return callbacks_.size();
    }

    // return number of elements that can be held in currently allocated storage
    int capacity(void)
    {
        return callbacks_.capacity();
    }

    int isempty(void)
    {
        return callbacks_.empty();  // returns true or false
    }

    // resize the vector to hold count elements
    void resize(int count)
    {
        callbacks_.resize(count);
    }

    void clear(void)
    {
        callbacks_.clear();
    }

    // remove the last item in the list
    void removelast(void)
    {
        callbacks_.pop_back();
    }

    /// Call all the registered callbacks.
    void callback() const
    {
        uint32_t handle=0;
        // iterate thru callback list and call each one
        for (const auto &cb: callbacks_)
        {
//           if (cb.handle == handle)
//            {
                cb.cb(val_);
//            }
        }

    }

private:
    /// Integer to pass to callbacks.
    uint32_t val_;
    /// List of callback functions.
    std::vector<cb_event_t> callbacks_;
};



extern "C" {

static void testfn(int n)
{
	printf("this is testfn calling\n");
	return;
}

}


/*
extern "C" static void testfn(int n)
{
    printf("this is testfn calling\n");
    return;
}
*/







//public stuff



EventDriver ed(0xDEADBEEF); //create event driver instance

void TestEventDriver(void)
{
	uint32_t myhandle;

    printf("InitEventDriver\n");
    printf("Size: %d\n",ed.size());
    myhandle=AddCallBack(&testfn);
    printf("Size: %d\n",ed.size());
    ExecCallBacks();
    printf("Size: %d\n",ed.size());
    RemoveCallBack(myhandle);
    printf("Size: %d\n",ed.size());


    if (ed.isempty() )  printf("we are now empty\n");
    else            printf("we are NOT empty\n");
}


void InitEventDriver(void)
{
	TestEventDriver();
}



uint32_t AddCallBack(const cb_t &cb)
{
//    ed.register_callback(&testfn,handle);
	uint32_t handle = ed.register_callback(cb);
	printf("my assigned handle:%d\n",handle);
	return handle;
}


void ExecCallBacks(void)
{
//	printf("ExecCallBacks()\n");
    ed.callback();
}


void RemoveCallBack(uint32_t handle)
{
	printf("RemoveCallBack()\n");
	ed.remove_callback(handle);
//	ed.removelast();
}



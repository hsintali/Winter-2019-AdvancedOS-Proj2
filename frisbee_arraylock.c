#include "types.h"
#include "stat.h"
#include "user.h"
#include "thread.h"

struct array_lock *lock;

uint threadCount;
uint passCount;

int token = 0;
int pass = 0;

void* work(void*);

int main(int argc, char *argv[])
{
	if(argc != 3)
    {
		printf(1, "Require 2 arguments.\n");
		exit();
    }
	
	threadCount = atoi(argv[1]);
    passCount = atoi(argv[2]);
	
    array_lock_init(lock, threadCount); 

	int startTime = uptime();

    int i = 0;	
    for (i = 0; i < threadCount; ++i)
    {
		thread_create(work, (void*)(i));
    }
	
    for (i = 0; i < threadCount; ++i)
    {
		wait();
    }
	
	int endTime = uptime();
	
	printf(1, "Simulation of Frisbee game has finished, %d rounds were played in total!\n", passCount);
    printf(1, "\nFrisbee game with array lock spent time: %d\n\n", endTime - startTime);

    exit();
}

void* work(void *args)
{	
	int threadID = (int)args;
	
	while(pass < passCount)
	{	
		int ticket = array_lock_acquire(lock);
        
		if(pass >= passCount)
		{
			array_lock_release(lock, ticket);
			break;
		}
		
		if(threadID == token)
		{
			token = threadID + 1;
			if(token == threadCount)
				token = 0;
		
			++pass;
			printf(1, "Pass number no: %d, Thread %d is passing the token to thread %d\n", pass, threadID, token);
		}
		
        array_lock_release(lock, ticket);
	}
	
	return 0;
}
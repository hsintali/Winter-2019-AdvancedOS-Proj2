#include "types.h"
#include "stat.h"
#include "user.h"
#include "thread.h"

struct seq_lock *lock;

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

    seqlock_init(lock);	
    //lock_init(lock->lk); 

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
    printf(1, "\nFrisbee game with spin lock spent time: %d\n\n", endTime - startTime);

    exit();
}

void* work(void *args)
{	
	int threadID = (int)args;
	int local_pass, local_token;
	int start;	
	//printf(1,"Start %d, %d\n",threadID, passCount);	
	do{
    		start = seqlock_reader_start(lock);
       	        local_pass=pass;
		local_token=token;
        }while(seqlock_reader_end(lock,start));
	
	while(local_pass < passCount)
	{	
		//while(seqlock_reader_end(lock,start))
		//{
        	        start = seqlock_reader_start(lock);
                	local_pass=pass;
			local_token=token;
        	//}
		if( local_pass >= passCount)
		{
			break;
		}
		
		if(seqlock_reader_end(lock,start)==0 && threadID == local_token)
		{
			seqlock_writer_acquire(lock);
			token = threadID + 1;
			if(token == threadCount)
				token = 0;
		
			++pass;
			printf(1, "Pass number no: %d, Thread %d is passing the token to thread %d\n", pass, threadID, token);
			
			local_pass = pass;
			local_token = token;

			seqlock_writer_release(lock);
			start = seqlock_reader_start(lock);
		}

	//	if(seqlock_reader_end(lock,start)==1)
	//	{
	//		printf(1, "TID%d %d T%d:%d P %d:%d\n", threadID, start, token, local_token, pass, local_pass );
	//	}
	}
	return 0;
}

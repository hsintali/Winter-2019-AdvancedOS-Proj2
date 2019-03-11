#include "types.h"
#include "user.h"
#include "x86.h"
#include "mmu.h"
#include "thread.h"


void thread_create(void*(*start_routine)(void*), void *arg)
{
	void* sp = malloc(PGSIZE);
	int id;
	
	id = clone(sp, PGSIZE);
	
	if(id == 0)
	{
		(*start_routine)(arg);
		exit();
	}
}

// spin lock
void lock_init(struct spin_lock *lk)
{
	lk->locked = 0;
}
void lock_acquire(struct spin_lock *lk)
{
	 while(xchg(&lk->locked, 1) != 0);
}
void lock_release(struct spin_lock *lk)
{
	xchg(&lk->locked, 0);
}

// array lock
uint fetch_and_inc(struct array_lock *lk)
{
	return xchg(&lk->last, lk->last + 1);
}
void array_lock_init(struct array_lock *lk, int size)
{	
	lk->size = size;
	lk->flags = (uint*)malloc(size * sizeof(uint));
	
	int i;
	for(i = 0; i < size; ++i)
	{
		lk->flags[i] = 0;
	}
	
	lk->flags[0] = 1;
	lk->last = 0;
}
int array_lock_acquire(struct array_lock *lk)
{
	int myplace = fetch_and_inc(lk);	
	while(lk->flags[myplace % lk->size] == must_wait);
	return myplace;
}
void array_lock_release(struct array_lock *lk, int myplace)
{
	lk->flags[myplace % lk->size] = must_wait;
	lk->flags[(myplace + 1) % lk->size] = has_lock;
}

// seq lock
void seqlock_init(struct seq_lock *sqlk)
{
	sqlk->locked=0;
	sqlk->seqcounter = 0;
	printf(1, "Init seqlock\n");
}
void seqlock_writer_acquire(struct seq_lock* sqlk)
{
	while(xchg(&sqlk->locked, 1) != 0);
	xchg(&sqlk->seqcounter, sqlk->seqcounter+1);
}
void seqlock_writer_release(struct seq_lock* sqlk)
{	
	xchg(&sqlk->seqcounter, sqlk->seqcounter+1);
	xchg(&sqlk->locked, 0);
}
unsigned seqlock_reader_start(struct seq_lock* sqlk)
{
	uint count;
	while(1)
	{
		count = sqlk->seqcounter;
		if(count%2==0)
			break;
	}
	return count;
}
unsigned seqlock_reader_end(struct seq_lock* sqlk, uint start)
{
	uint count;
	count=sqlk->seqcounter;
	if(count != start) 
		return 1; //retry reading
	else 
		return 0; 
}
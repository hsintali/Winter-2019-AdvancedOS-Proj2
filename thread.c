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
	while(lk->flags[myplace % lk->size] == must_wait) { printf(1, ""); }; // avoid to break loop
	return myplace;
}
void array_lock_release(struct array_lock *lk, int myplace)
{
	lk->flags[myplace % lk->size] = must_wait;
	lk->flags[(myplace + 1) % lk->size] = has_lock;
}
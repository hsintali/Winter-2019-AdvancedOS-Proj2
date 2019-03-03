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
struct spin_lock
{
	uint locked;
};

void thread_create(void *(*start_routine)(void*), void *arg);

// spin lock
void lock_init(struct spin_lock*);
void lock_acquire(struct spin_lock*);
void lock_release(struct spin_lock*);
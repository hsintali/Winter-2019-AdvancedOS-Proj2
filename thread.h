struct spin_lock
{
	uint locked;
};

#define must_wait 0
#define has_lock 1
struct array_lock
{
	uint *flags;
	uint last;
	int size;
};

void thread_create(void *(*start_routine)(void*), void *arg);

// spin lock
void lock_init(struct spin_lock*);
void lock_acquire(struct spin_lock*);
void lock_release(struct spin_lock*);

// array lock
uint fetch_and_inc(struct array_lock *);
void array_lock_init(struct array_lock *, int);
int array_lock_acquire(struct array_lock *);
void array_lock_release(struct array_lock *, int);
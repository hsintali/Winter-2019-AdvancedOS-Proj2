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

struct seq_lock
{
	uint locked;
	uint seqcounter;
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

// seq lock
void seqlock_init(struct seq_lock*);
void seqlock_writer_acquire(struct seq_lock *);
void seqlock_writer_release(struct seq_lock *);
uint seqlock_reader_start(struct seq_lock *);
uint seqlock_reader_end(struct seq_lock *, uint);
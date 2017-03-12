#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum{
	bfree = 0,
	full, mutex
} sem_type;

typedef enum{
	p,v
} sem_op;

struct mymsgbuf{
	long mtype;
	struct mes{
		pid_t pid;
		sem_type sem_num;
		sem_op op;
	} msg;
};

//void error(int cond, const char * message);
//void exec_sem (sem_op op, sem_type s_type, int msqid);

static inline void error(int cond, const char * message) {
	if (cond) {
		printf("%s", message);
		unlink("lock");
		exit(-1);
	}
}

static inline void destroy_comm (int msqid) {
	msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
	unlink("lock");
}

void exec_sem (sem_op op, sem_type s_type, int msqid);

typedef int Semaphore;

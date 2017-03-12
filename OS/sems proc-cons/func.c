#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "types.h"	

void exec_sem (sem_op op, sem_type s_type, int msqid) {
	const int buf_len = sizeof(struct mes);
	pid_t pid = getpid();
	struct mymsgbuf buf = {1, {pid, s_type, op}};
    
	if (msgsnd(msqid, (struct msgbuf *) &buf, buf_len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		destroy_comm (msqid);
		exit(-1);
    }
    if (msgrcv(msqid, (struct msgbuf *) &buf, buf_len, pid, 0) < 0) {
		printf("Can\'t receive message from queue\n");
		destroy_comm(msqid);
		exit(-1);
	}
}

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "types.h"

int make_sem_op(sem_op op, Semaphore * sem_ptr) {
	if (op == v) {
		*sem_ptr += 1;
		return 0;
	}
	assert(*sem_ptr >= 0);
	if (*sem_ptr == 0)
		return 1;
	else
		*sem_ptr -= 1;
	return 0;
}

int main() {

	Semaphore sems[3] = {5, 0, 1};
	int key, msqid;
	struct mymsgbuf buf;
	int buf_len = sizeof(buf.msg);
    //char pathname[] = "lock";
    char shmem_path[] = "server.c";

	printf("Server is in run\n");
    
    //TODO edit connection to message queue
	if ((key = ftok(shmem_path, 0)) < 0) {
        printf("S: Can't attach to queue\n");
        exit(-1);
    }

	if ((msqid = msgget(key, 0)) < 0) {
		printf("S: Can\'t find message queue\n");
		exit(-1);
	}
		
	int blocked_sem = -1;
	int blocked = 0, new_blocked = 0;
	
	//fprintf(stderr, "S: Wait to receive\n");	
	if (msgrcv(msqid, (struct msgbuf *) &buf, buf_len, -2, 0) < 0) {
		printf("S: Can\'t receive message from queue\n");
		destroy_comm (msqid);
		exit(-1);
	}
	
	while (buf.mtype != 2) {
		int send_flag = 0;
		if (blocked) {
			new_blocked = make_sem_op(buf.msg.op, sems + buf.msg.sem_num);
			if (new_blocked)
				break;
			buf.mtype = buf.msg.pid;
			send_flag = 1;
		}
		else {
			blocked = make_sem_op(buf.msg.op, sems + buf.msg.sem_num);
			if (blocked) {
				blocked = buf.msg.pid;
				blocked_sem = buf.msg.sem_num;
				fprintf(stderr, "S: Block\n");
			}
			else {
				buf.mtype = buf.msg.pid;
				send_flag = 1;
			}			
		}
		
		if (blocked && buf.msg.op == v && buf.msg.sem_num == blocked_sem) {	//unblock
			fprintf(stderr, "S: unblock\n");
			sems[blocked_sem] -= 1;
			buf.mtype = blocked; 
			blocked = 0;
			blocked_sem = -1;
			send_flag = 2;
		}

		while (send_flag--) {
			//fprintf(stderr, "S: Wait to send\n");			
			
			if (msgsnd(msqid, (struct msgbuf *) &buf, buf_len, 0) < 0) {
				printf("S: Can\'t send message to queue\n");
				destroy_comm (msqid);
				exit(-1);
		    }
		    buf.mtype = buf.msg.pid;
	    }
		//fprintf(stderr, "S: Wait to receive\n");			
		if (msgrcv(msqid, (struct msgbuf *) &buf, buf_len, -2, 0) < 0) {
			printf("S: Can\'t receive message from queue\n");
			destroy_comm (msqid);
			exit(-1);
		}
    }
	destroy_comm (msqid);
	printf("Server is down\n");

	return 0;
}

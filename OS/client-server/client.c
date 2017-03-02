#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    struct mymsgbuf {
    	long mtype;
    	struct mes {
			pid_t pid;
			int data;
		} msg;
    } buf;
    
    int msqid, res;
    char pathname[]="server.c";
    key_t  key;
    pid_t my_pid = getpid();
    const int buf_len = sizeof(buf.msg);

    /* Create or attach message queue  */
    
    key = ftok(pathname, 0);
    
    if ((msqid = msgget(key, 0)) < 0){
        printf("Can\'t find message queue\n");
        exit(-1);
    }
    
    printf("Queue id is %d\n", msqid); 

	int i = 0;
    for (i = 0; i < 10; i++) {
        res = fork();
        if (res < 0) {
            printf("Can\'t fork!\n");
            exit(-1);
        }
        else if (res == 0)
            break;
    }

    buf.msg.data = 7*i*i + 5*i + 3;
    buf.msg.pid = my_pid;
    buf.mtype = 1;
    
    if (msgsnd(msqid, (struct msgbuf *) &buf, buf_len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(-1);
    }
    
    if ( msgrcv(msqid, (struct msgbuf *) &buf, buf_len, my_pid, 0) < 0) {
		printf("Can\'t receive message from queue\n");
		exit(-1);
	}
	printf("Answer is %d\n", buf.msg.data);
	return 0;
}

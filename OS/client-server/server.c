#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    struct mymsgbuf {
    	long mtype;
    	struct mes {
			pid_t pid;
			int data;
		} msg;
    } buf;

    int msqid;
    key_t key;
    const int buf_len = sizeof(buf.msg);
    char pathname[]="server.c";
    
    
    /* Create or attach message queue  */
    
    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't attach to queue\n");
        exit(-1);
    }
    
    if ((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) < 0){
       printf("Can\'t create msqid\n");
       exit(-1);
    }
    
    printf("Queue id is %d\n", msqid);
    
    if (msgrcv(msqid, (struct msgbuf *) &buf, buf_len, -2, 0) < 0) {
         printf("Can\'t receive message from queue\n");
         exit(-1);
    }
    
    while (buf.mtype != 2) {
		buf.msg.data *= buf.msg.data;
		buf.mtype = buf.msg.pid;
		
		if (msgsnd(msqid, (struct msgbuf *) &buf, buf_len, 0) < 0) {
			printf("Can\'t send message to queue\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
			exit(-1);
        }

		if (msgrcv(msqid, (struct msgbuf *) &buf, buf_len, -2, 0) < 0) {
		     printf("Can\'t receive message from queue\n");
		     exit(-1);
		}
    }
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
    return 0;
}


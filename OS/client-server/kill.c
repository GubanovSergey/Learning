#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    struct mymsgbuf {
    	long mtype;
    	struct mes {
			pid_t pid;
			int data;
		} msg;
    } buf;
    
    int msqid;
    char pathname[]="server.c";
    key_t  key;
   	const int buf_len = sizeof(buf.msg);

    /* Create or attach message queue  */
    
    key = ftok(pathname, 0);
    
    if ((msqid = msgget(key, 0)) < 0){
        printf("Can\'t find message queue\n");
        exit(-1);
    }
    
    buf.mtype = 2;
    
    if (msgsnd(msqid, (struct msgbuf *) &buf, buf_len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(-1);
    }
    return 0;
}

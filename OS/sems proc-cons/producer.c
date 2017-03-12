#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"	

int main(void) {
    int msqid;
    int fd;
    key_t key;
    //char pathname[] = "lock";
    char shmem_path[] = "server.c";
        
    key = ftok(shmem_path, 0);
    error(key < 0, "Can't attach to queue\n");

    errno = 0;
    
    if ((fd = open ("lock", O_RDWR | O_CREAT | O_EXCL)) >= 0) {
		if ((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) < 0){
			error (errno != EEXIST, "Can\'t create msqid\n");
		}
		int pid = fork();
		error (pid < 0, "Can't fork!\n");
		if (pid  == 0) {//Child - start a server
    		char * a = "./server";
    		int var = execlp(a, a, NULL);
    		error (var < 0, "Can't exec\n");
		}
	}
	else if (errno != EEXIST){
         printf("Can\'t locate lock-file\n");
         exit(-1);
	}
	else {
		msqid = msgget(key, 0);
		error (msqid < 0, "Can\'t find message queue\n");
    }
    
    for (int i = 0; i < 15; i++) {
    	//fprintf(stderr, "Wait for bfree\n");
    	exec_sem (p, bfree, msqid);
    	//fprintf(stderr, "Wait for mutex\n");
    	exec_sem (p, mutex, msqid);
    	fprintf(stderr, "Put item in queue\n");    	
    	exec_sem (v, mutex, msqid);
    	exec_sem (v, full, msqid);
    	sleep(0.5);
	}
	    
    return 0;
}


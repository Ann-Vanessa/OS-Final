#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "thread_pool.h"
#include <bits/getopt_core.h> // for optarg

#define DEFAULT_PORT 10000
#define DEFAULT_MAX_THREADS 4
#define DEFAULT_MAX_BUF 8
// #define DEFAULT_ROOT "."
char default_root[] = "."; 

int main(int argc, char *argv[]) {
    int d;
    int max_threads = DEFAULT_MAX_THREADS;
    int max_buf = DEFAULT_MAX_BUF;
    int port = DEFAULT_PORT;
    char *root_dir = default_root;
	int *new_conn;
    
    while ((d = getopt(argc, argv, "d:p:t:b:")) != -1)
	switch (d) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		max_threads = atoi(optarg);
		break;
	case 'b':
		max_buf = atoi(optarg);
		break;
	default:
		// printf("%c",d);
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffer]\n");
	    exit(1);
	}

	srand(time(NULL));
	// printf("%d %d\n",MAXTH,MAXBUF);
    thread_pool *pool = pool_init(max_threads,max_buf);
    printf("Testing threadpool of %d threads.\n", get_max_threads(pool));
    // run out of this directory
    chdir_or_die(root_dir);
    // int *t=malloc((MAXBUF+1)*sizeof(int));
    // int *c=calloc(MAXBUF+1,sizeof(int));
    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
	// pthread_t *worker_thread = malloc((MAXBUF+1)*sizeof(pthread_t));
    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	new_conn = malloc(1);
	*new_conn = conn_fd;
    if(conn_fd!=-1) {
        printf("Connection established: %d\n",conn_fd);
        pool_add_task(pool,request_handle, (void *)new_conn);
        // printf("Schedule: %d\n Head: %d\n Tail: %d\n",get_schedule(pool),get_head(pool),get_tail(pool));
        }
    }
  // printf("All scheduled!\n");

    pool_wait(pool);
    pool_destroy(pool);


    printf("Done.");
					
    return 0;
}

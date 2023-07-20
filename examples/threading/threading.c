#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

pthread_mutex_t mutex_g;

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    
	struct thread_data* thread_func_args = (struct thread_data *) thread_param;

	//if(!thread_func_args->thread_complete_success)
	//	printf("sucess: %d\n",thread_func_args->thread_complete_success);

	printf("wait to obtain : %d\n",thread_func_args->wait_to_obtain_ms);

	usleep(thread_func_args->wait_to_obtain_ms);

    //int rc =pthread_mutex_lock(&thread_func_args->mutex);
	int rc =pthread_mutex_lock(&mutex_g);

	if(rc!=0){
		perror("pthread_mutex lock");
		return thread_param;
	}
	
	printf("wait to release : %d\n",thread_func_args->wait_to_release_ms);
    sleep(thread_func_args->wait_to_release_ms);

	//rc = pthread_mutex_unlock(&thread_func_args->mutex);
	rc = pthread_mutex_unlock(&mutex_g);


	if(rc!=0){
        perror("pthread mutex unlock");
        return thread_param;
    }

	thread_func_args->thread_complete_success=true;

	printf("func ended\n");

	return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	struct thread_data *t_data;

	t_data = (struct thread_data *)malloc(sizeof(struct thread_data));

	mutex_g=*mutex;
	if (pthread_mutex_init(&mutex_g,NULL)!=0){
		perror("pthread mutex init:");
		return false;
	}
	t_data->thread_complete_success=false;
	t_data->wait_to_release_ms=wait_to_release_ms;
	t_data->wait_to_obtain_ms=wait_to_obtain_ms;

	int result = pthread_create(thread, NULL, threadfunc, (void *)t_data);

	/*if(pthread_join(*thread,NULL)!=0){
		perror("pthread_join");
	}//*/	

	//free(t_data);

	if(result != 0)
			return false;
	else
			return true;
}


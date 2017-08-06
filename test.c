#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define RESOURCE_WITH_READERS   1
#define RESOURCE_FREE           0
#define RESOURCE_WITH_WRITERS  -1

int numReaders = 0;
int numWriters = 0;
int resourceFlag =0;
int sharedVariable = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
pthread_cond_t c_read = PTHREAD_COND_INITIALIZER; /* consumer waits on this cond var */
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER; /* producer waits on this cond var */

void * Reader_thread(void * param);
void * Writer_thread(void * param);


int main()
{
	// Declare the threads
	pthread_t readThread[5];
	pthread_t writeThread[5];
	int i;

	for (i = 0; i < 5 ; i++)
	{
		// create the threads
		if (pthread_create(&readThread[i], NULL, Reader_thread, NULL) != 0)
		{
			fprintf(stderr, "Unable to create reader thread\n");
			exit(1);
		}

		if (pthread_create(&writeThread[i], NULL, Writer_thread, NULL) != 0)
		{
			fprintf(stderr, "Unable to create writer thread\n");
			exit(1);
		}

		// join the threads
		pthread_join (readThread[i], NULL);
		printf ("%dth Reader thread Joined!\n",i);
		pthread_join (writeThread[i], NULL);
		printf("%dth Writer thread Joined\n", i);

	}

	printf("Parent exiting\n");

	return 0;
}

void * Reader_thread(void * param)
{
	// Enter critical section
	pthread_mutex_lock (&m);

	while (resourceFlag < 0) pthread_cond_wait (&c_read, &m);
	resourceFlag ++ ;
	numReaders ++ ;
	pthread_mutex_unlock(&m);

	// Reader portion
	printf("The value of Shared variable X = %d\n", sharedVariable);
	printf("The number of readers reading Shared Variable X is %d\n", numReaders);

	// Exit critical section
	pthread_mutex_lock (&m);
	resourceFlag -- ;
	numReaders -- ;

	if (numReaders == 0)
	pthread_cond_signal (&c_write);

	pthread_mutex_unlock(&m);

}

void * Writer_thread(void * param)
{
	// Enter critical section
	pthread_mutex_lock (&m);

	while (resourceFlag > 0) pthread_cond_wait (&c_write, &m);
	resourceFlag ++ ;
	numWriters ++;

	pthread_mutex_unlock(&m);

	// Writer portion
	sharedVariable += 10;
	printf("The value of Shared variable X = %d\n", sharedVariable);
	printf("The number of readers reading Shared Variable X is %d\n", numReaders);

	// Exit critical section
	pthread_mutex_lock (&m);
	resourceFlag -- ;
	numWriters -- ;

	if (numWriters == 0)
	pthread_cond_signal (&c_read);
	pthread_mutex_unlock(&m);
}

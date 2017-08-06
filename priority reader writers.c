#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


#define RESOURCE_WITH_READERS 1
#define RESOURCE_FREE 0
#define RESOURCE_WITH_WRITERS -1

int numReaders, numWriters;
int resourceFlag;
int sharedVariable = 0;


pthread_mutex_t m = PTHREAD_MUTEX_INITIALISER;
pthread_cond_t c_read = PTHREAD_COND_INTIALISER;
pthread_cond_t c_write = PTHREAD_COND_INTIALISER;

void * Reader_thread(void * param);
void * Writer_thread(void * param);


int main()
{
	// Declare the threads
	pthread_t readThread;
	pthread_t writeThread;
	int i;

	for (i = 0; i < 20 ; i++)
	{
		// create the threads
		if (pthread_create(&readThread, NULL, Reader_thread, NULL) != 0)
		{
			fprintf(stderr, "Unable to create reader thread\n");
			exit(1);
		}

		if (pthread_create(&writeThread, NULL, Writer_thread, NULL) != 0)
		{
			fprintf(stderr, "Unable to create writer thread\n");
			exit(1);
		}

		// join the threads and exit

		pthread_join (Reader_thread, NULL);
		ptread_join (Writer_thread, NULL);

	}

	printf("Parent exiting\n");

	return 0;
}

int Reader_thread()
{
	// Enter critical section
	pthread_mutex_lock (&m);

	while (resourceFlag < 0)
	pthread_cond_wait (m, c_read);
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

int Writer_thread()
{
	// Enter critical section
	pthread_mutex_lock (&m);

	while (resourceFlag > 0)
	pthread_cond_wait (m, c_write);
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

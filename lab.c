#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

typedef struct ConsumerDataStruct{
    int id;
}ConsumerData;

volatile int item = -1, available = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

void* producer(void *ws){
    int n;

    printf("Enter numbers (0 to quit)\n");

    do{
        scanf("%d", &n);

        pthread_mutex_lock(&mutex);

        item = n;
        available = 1;

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condvar);
    }while(n != 0);

    return NULL;
}

void* consumer(void *ws){
    ConsumerData *data = (ConsumerData *)ws;

    do{
        pthread_mutex_lock(&mutex);

        while(available == 0){
            pthread_cond_wait(&condvar, &mutex);
        }

        available = 0;

        printf("Consumer %d says 'You typed %d'\n", data->id,item);
        //sleep(2);
        pthread_mutex_unlock(&mutex);
    }while(item != 0);
}

int main(int argc, char *argv[]) {

    pthread_t prod_thread;

    ConsumerData cons_data[10];
    pthread_t cons_threads[10];

    pthread_create(&prod_thread, NULL, producer, NULL);

    for(int i = 0; i<10; i++) {
        cons_data[i].id = i;

        pthread_create(&(cons_threads[i]), NULL, consumer, &cons_data[i]);
    }

    pthread_join(prod_thread, NULL);

    for(int i = 0; i<10; i++) {
        pthread_cancel(cons_threads[i]);
    }

    return EXIT_SUCCESS;
}

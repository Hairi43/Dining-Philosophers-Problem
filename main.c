/*
 *  Dining Philosophers Problem in C
 *  version with waiter/arbiter.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <windows.h>

int numberOfPhilosophers;
/* if 'end' equals 1 then threads end it's work */
int end;

/* pointer to global array of semaphores and one semaphore as a waiter */
sem_t *forks;
sem_t waiter;
/* mutex for reading and writing to 'end' variable */
pthread_mutex_t locker;
/* mutex for using printf() in thread-safe mode */
pthread_mutex_t printer;

/* function running in separate thread for checking if user wants to end program */
void *waitForInput(void *arg) {
    int c;
    while (1) {
        if ((c = getchar()) == 'q') {
            pthread_mutex_lock(&printer);
            printf("Closing program. \nPlease wait patiently for each thread to end it's work \n");
            pthread_mutex_unlock(&printer);
            pthread_mutex_lock(&locker);
            end = 1;
            pthread_mutex_unlock(&locker);
            break;
        }
    }
    return 0;
}

int leftFork(int i) {
    return i;
}

int rightFork(int i) {
    return (i+1) % numberOfPhilosophers;
}

void eat(int id) {
    pthread_mutex_lock(&printer);
    printf("Philosopher %d is eating\n", id);
    pthread_mutex_unlock(&printer);
    Sleep(11000);
}

void think(int id) {
    pthread_mutex_lock(&printer);
    printf("Philosopher %d is thinking\n", id);
    pthread_mutex_unlock(&printer);
    Sleep(8000);
}

void *dine(void *idptr) {
    /* philosopher's id
     *
     * arg of function dine() is an 8 byte address on 64-bit architecture
     * so first void* is cast to int* (to know that at this address is an integer value)
     * and then pointer dereference is used to get value at that address.
     */
    int id = *((int *) idptr);

    while (1) {
        pthread_mutex_lock(&locker);
        if (end != 0) {
            break;
        }
        pthread_mutex_unlock(&locker);

        /* philosopher's thinking */
        think(id);

        /* waiter semaphore has counter of Philosophers / 2 */
        sem_wait(&waiter);
        sem_wait(&forks[rightFork(id)]);
        sem_wait(&forks[leftFork(id)]);

        /* philosopher's eating */
        eat(id);

        sem_post(&forks[leftFork(id)]);
        sem_post(&forks[rightFork(id)]);
        sem_post(&waiter);
        pthread_mutex_lock(&printer);
        printf("Philosopher %d stopped eating\n", id);
        pthread_mutex_unlock(&printer);
    }
    /* unlock global variable 'end' mutex so that other threads can end their work */
    pthread_mutex_unlock(&locker);
    pthread_mutex_lock(&printer);
    printf("Philosopher %d stopped working\n", id);
    pthread_mutex_unlock(&printer);
    return 0;
}

/* parse number of philosophers provided as program's argument */
int parseArgs(int argc, char *argv[]) {

    if (argc == 2) {
        numberOfPhilosophers = atoi(argv[1]);
    }
    else if (argc > 2){
        printf("Too many arguments. Only need a number of philosophers.\n");
        return 1;
    }
    else {
        printf("Add number of philosophers as argument\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    /* parse program's arguments */
    if (parseArgs(argc, argv) != 0) {
        return 1;
    }

    /* global variable with purpose for closing program */
    end = 0;
    pthread_mutex_init(&printer, NULL);
    pthread_mutex_init(&locker, NULL);


    /* create semaphores */
    printf("Number of philosophers: %d\n", numberOfPhilosophers);
    /* allocate memory for semaphores and assign it to the global pointer */
    forks = malloc(sizeof(sem_t) * numberOfPhilosophers);
    if (forks == NULL) {
        printf("Allocation failed");
        return 1;
    }
    /* waiter semaphore has value of N / 2, because that's the largest
     * amount of philosopher's that can eat simultaneously */
    sem_init(&waiter, 0, numberOfPhilosophers / 2);
    for (int i = 0; i < numberOfPhilosophers; i++) {
        if (sem_init(&forks[i], 0, 1) != 0) {
            printf("Semaphore wasn't closed correctly\n");
            return 1;
        }
    }

    /* initialize threads */
    pthread_t threads[numberOfPhilosophers];
    pthread_t endProgram;
    pthread_create(&endProgram, NULL, waitForInput, NULL);

    int philosopherIds[numberOfPhilosophers];
    for (int i = 0; i < numberOfPhilosophers; i++) {
        philosopherIds[i] = i;
    }

    /* create threads and pass jobs to them */
    for (int id = 0; id < numberOfPhilosophers; id++) {
        if (pthread_create(&threads[id], NULL, dine, &philosopherIds[id]) != 0) {
            pthread_mutex_lock(&printer);
            printf("Error creating thread\n");
            pthread_mutex_unlock(&printer);
            pthread_mutex_lock(&locker);
            end = 1;
            pthread_mutex_unlock(&locker);
        }
    }

    /*
    end threads life. pthread_join returns 0 if thread was closed without problems
    and 1 if thread was closed incorrectly
    */
    for (int id = 0; id < numberOfPhilosophers; id++) {
        if (pthread_join(threads[id], NULL) != 0) {
            pthread_mutex_lock(&printer);
            printf("Error joining thread\n");
            pthread_mutex_unlock(&printer);
        }
    }

    pthread_join(endProgram, NULL);

    printf("\nAll threads have terminated.\n");

    /* delete previously created semaphores */
    if (sem_destroy(&waiter) != 0) {
        printf("Waiter semaphore wasn't closed correctly\n");
    }
    for (int i = 0; i < numberOfPhilosophers; i++) {
        if (sem_destroy(&forks[i]) != 0) {
            printf("Fork semaphores wasn't closed correctly\n");
        }
    }
    /* free allocated memory */
    free(forks);
    pthread_mutex_destroy(&locker);
    pthread_mutex_destroy(&printer);
    return 0;
}

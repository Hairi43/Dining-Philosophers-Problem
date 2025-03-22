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

/* pointer to global array of semaphores and one semaphore as a waiter */
sem_t *forks;
sem_t waiter;

/* function running in separate thread for checking if user wants to end program */
void *waitForInput(void *threads) {
    int threadsArr = *(int *) threads;
    char c;
    while (1) {
        if ((c = getchar()) == 'q') {
            printf("Closing program");
            // exit(0);
            for (int id = 0; id < numberOfPhilosophers; id++) {
                if (pthread_join(threadsArr[id], NULL) != 0) {
                    printf("Error joining thread\n");
                }
            }
        }
    }
}

int leftFork(int i) {
    return i;
}

int rightFork(int i) {
    return (i+1) % numberOfPhilosophers;
}

void eat(int id) {
    printf("Philosopher %d is eating\n", id);
    Sleep(11000);
}

void think(int id) {
    printf("Philosopher %d is thinking\n", id);
    Sleep(8000);
}

void *dine(void *idptr) {
    /* philosopher's id */
    int id = *(int *) (idptr);
    while (1) {
        /* philosopher's thinking */
        think(id);

        sem_wait(&waiter);
        sem_wait(&forks[rightFork(id)]);
        sem_wait(&forks[leftFork(id)]);

        /* philosopher's eating */
        eat(id);

        sem_post(&forks[leftFork(id)]);
        sem_post(&forks[rightFork(id)]);
        sem_post(&waiter);
        printf("Philosopher %d stopped eating\n", id);
    }
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


    /* create semaphores */
    printf("Number of philosophers: %d\n", numberOfPhilosophers);
    /* allocate memory for semaphores and assign it to the global pointer */
    forks = malloc(sizeof(sem_t) * numberOfPhilosophers);
    if (forks == NULL) {
        printf("Allocation failed");
        return 1;
    }
    sem_init(&waiter, 0, numberOfPhilosophers);
    for (int i = 0; i < numberOfPhilosophers; i++) {
        if (sem_init(&forks[i], 0, 1) != 0) {
            printf("Semaphore wasn't closed correctly\n");
            return 1;
        }
    }

    /* initialize threads */
    pthread_t threads[numberOfPhilosophers];
    pthread_t endProgram;
    pthread_create(&endProgram, NULL, waitForInput, &threads);

    int philosopherIds[numberOfPhilosophers];
    for (int i = 0; i < numberOfPhilosophers; i++) {
        philosopherIds[i] = i;
    }

    /* create threads and pass jobs to them */
    for (int id = 0; id < numberOfPhilosophers; id++) {
        if (pthread_create(&threads[id], NULL, dine, &philosopherIds[id]) != 0) {
            printf("Error creating thread\n");
            exit(1);
        }
    }

    pthread_join(endProgram, NULL);

    /*
    end threads life. pthread_join returns 0 if thread was closed without problems
    and 1 if thread was closed incorrectly
    */
    for (int id = 0; id < numberOfPhilosophers; id++) {
        if (pthread_join(threads[id], NULL) != 0) {
            printf("Error joining thread\n");
        }
    }

    printf("All threads have terminated.\n");

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
    return 0;
}
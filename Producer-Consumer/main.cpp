#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define EMPTY 0
#define FULL 1
#define MUTEX 2
#define BUFF 32

int main() {

  // initializing shared memory
  creat("shm_file", 0600);
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, BUFF * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
  if (shmid == -1) {
    if (errno != EEXIST) {
      std::cout << "ERROR: Cannot create shared mem segment!!\n";
      exit(1);
    } else {
      shmid = shmget(key, 0, 0600);
    }
  }

  void *shmp = shmat(shmid, NULL, 0);
  if (shmp == (void *)-1) {
    perror("Shared memory attach");
    exit(1);
  }

  int *nums = (int *)shmp;
  for (int i = 0; i < BUFF; ++i) {
    nums[i] = 0;
  }

  // initializing semaphore
  int sem_key = ftok(".", 'S');
  int semid;
  if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
    perror("semget");
  }

  struct sembuf semops;

  if (semctl(semid, EMPTY, SETVAL, 0) == -1) {
    perror("semctl");
  }
  if (semctl(semid, FULL, SETVAL, BUFF) == -1) {
    perror("semctl");
  }
  if (semctl(semid, MUTEX, SETVAL, 1) == -1) {
    perror("semctl");
  }

  // creating processes

  // Producer
  if (fork() == 0) {
    if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }

    semops.sem_num = FULL;
    semops.sem_op = -1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    semops.sem_num = MUTEX;
    semops.sem_op = -1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    for (int i = 0; i < 100; ++i) {
      nums[i % BUFF] = rand() % 10 + 1;

      semops.sem_num = EMPTY;
      semops.sem_op = 1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }

    semops.sem_num = MUTEX;
    semops.sem_op = 1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
    }
    exit(0);
  }

  // Consumer
  if (fork() == 0) {
    if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }

    semops.sem_num = EMPTY;
    semops.sem_op = -1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    semops.sem_num = MUTEX;
    semops.sem_op = -1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    for (int i = 0; i < 100; ++i) {
      std::cout << nums[i % BUFF] << " ";

      semops.sem_num = FULL;
      semops.sem_op = 1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }

    semops.sem_num = MUTEX;
    semops.sem_op = 1;
    if (semop(semid, &semops, 1) == -1) {
      perror("semop");
    }

    if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
    }
    exit(0);
  }

  wait(NULL);
  wait(NULL);

  return 0;
}

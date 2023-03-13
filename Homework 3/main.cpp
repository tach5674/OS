#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int main() {
  creat("shm_file", 0600);
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
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

  int *num = (int *)shmp;
  *num = 0;

  union semun semarg;
  struct sembuf semops;
  key_t sem_key;
  int semid;

  sem_key = ftok(".", 'S');
  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
    perror("semget");
  }

  semarg.val = 0;
  if (semctl(semid, 0, SETVAL, semarg) == -1) {
    perror("semctl");
  }
  semops.sem_op = 1;

  if (fork() == 0) {
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }
    for (int i = 0; i < 100; ++i) {
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }
    return 0;
  }

  if (fork() == 0) {
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }
    for (int i = 0; i < 100; ++i) {
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }
    return 0;
  }

  wait(NULL);
  wait(NULL);

  *num = semctl(semid, 0, GETVAL, semarg);
  std::cout << *num;

  if (semctl(semid, 0, IPC_RMID, semarg) == -1) {
    perror("semctl");
  }

  if (shmdt(shmp) == -1) {
    perror("shmdt");
    exit(1);
  }
  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("shmctl");
    exit(1);
  }

  return 0;
}

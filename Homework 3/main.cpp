#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

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

  struct sembuf semops;
  key_t sem_key;
  int semid;

  sem_key = ftok(".", 'S');
  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
    perror("semget");
  }

  if (semctl(semid, 0, SETVAL, 1) == -1) {
    perror("semctl");
  }

  if (fork() == 0) {
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }
    for (int i = 0; i < 10000; ++i) {
      semops.sem_op = -1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }

      ++(*num);

      semops.sem_op = 1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }
    if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
    }
    return 0;
  }

  if (fork() == 0) {
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }
    for (int i = 0; i < 10000; ++i) {
      semops.sem_op = -1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }

      ++(*num);

      semops.sem_op = 1;
      if (semop(semid, &semops, 1) == -1) {
        perror("semop");
      }
    }
    if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
    }
    return 0;
  }

  wait(NULL);
  wait(NULL);

  std::cout << *num;

  if (semctl(semid, 0, IPC_RMID, NULL) == -1) {
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

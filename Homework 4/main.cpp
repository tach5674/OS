#include <iostream>
#include <signal.h>
#include <string>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  struct sembuf semops[4];
  key_t sem_key;
  int semid;

  sem_key = ftok(".", 'S');
  if ((semid = semget(sem_key, 4, 0666 | IPC_CREAT)) == -1) {
    perror("semget");
  }

  if (semctl(semid, 0, SETVAL, 1) == -1) {
    perror("semctl");
  }
  if (semctl(semid, 1, SETVAL, 0) == -1) {
    perror("semctl");
  }
  if (semctl(semid, 2, SETVAL, 0) == -1) {
    perror("semctl");
  }
  if (semctl(semid, 3, SETVAL, 0) == -1) {
    perror("semctl");
  }

  std::string str;
  std::cin >> str;

  pid_t pidT = fork();

  if (pidT == 0) {
    if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }
    while (1) {
      semops[1].sem_op = -1;
      if (semop(semid, &semops[1], 1) == -1) {
        perror("semop");
      }

      // sleep(5);
      std::cout << "T";

      semops[0].sem_op = 1;
      if (semop(semid, &semops[0], 1) == -1) {
        perror("semop");
      }
    }
    return 0;
  }

  pid_t pidP = fork();

  if (pidP == 0) {
    if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }

    while (1) {
      semops[1].sem_op = -1;
      if (semop(semid, &semops[1], 1) == -1) {
        perror("semop");
      }

      // sleep(5);
      std::cout << "P";
      semops[0].sem_op = 1;
      if (semop(semid, &semops[0], 1) == -1) {
        perror("semop");
      }
    }
    return 0;
  }

  pid_t pidM = fork();

  if (pidM == 0) {
    if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
    }

    while (1) {
      semops[1].sem_op = -1;
      if (semop(semid, &semops[1], 1) == -1) {
        perror("semop");
      }

      // sleep(5);
      std::cout << "M";

      semops[0].sem_op = 1;
      if (semop(semid, &semops[0], 1) == -1) {
        perror("semop");
      }
    }
    return 0;
  }

  for (int i = 0; i < str.length(); ++i) {
    semops[0].sem_op = -1;
    if (semop(semid, &semops[0], 1) == -1) {
      perror("semop");
    }
    if (str[i] == 't') {
      semops[1].sem_op = 1;
      if (semop(semid, &semops[1], 1) == -1) {
        perror("semop");
      }
    }
    if (str[i] == 'p') {
      semops[2].sem_op = 1;
      if (semop(semid, &semops[2], 1) == -1) {
        perror("semop");
      }
    }
    if (str[i] == 'm') {
      semops[3].sem_op = 1;
      if (semop(semid, &semops[3], 1) == -1) {
        perror("semop");
      }
    }
  }
  if(str[str.length()-1] == 't'){
    kill(pidP, SIGTERM);
    kill(pidM, SIGTERM);
    semops[0].sem_op = -1;
    if (semop(semid, &semops[0], 1) == -1) {
      perror("semop");
    }
    kill(pidT, SIGTERM);
  }
  if(str[str.length()-1] == 'p'){
    kill(pidT, SIGTERM);
    kill(pidM, SIGTERM);
    semops[0].sem_op = -1;
    if (semop(semid, &semops[0], 1) == -1) {
      perror("semop");
    }
    kill(pidP, SIGTERM);
  }
  if(str[str.length()-1] == 'm'){
    kill(pidP, SIGTERM);
    kill(pidT, SIGTERM);
    semops[0].sem_op = -1;
    if (semop(semid, &semops[0], 1) == -1) {
      perror("semop");
    }
    kill(pidM, SIGTERM);
  }
  
  return 0;
}

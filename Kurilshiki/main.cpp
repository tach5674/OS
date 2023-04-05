#include <iostream>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TABACCO 0
#define MATCHES 1
#define PAPER 2
#define BARMEN_SEM 3

int sem_id;

void barmen(int sem_id, pid_t s1, pid_t s2, pid_t s3) {
  std::string inputs;
  std::cin >> inputs;

  std::cout << "Barmen is searching items..." << std::endl;

  for (auto ch : inputs) {

    if (ch == 't') {
      struct sembuf found_tabacco {
        TABACCO, 1, 0
      };
      semop(sem_id, &found_tabacco, 1);
    }

    if (ch == 'p') {
      struct sembuf found_paper {
        PAPER, 1, 0
      };
      semop(sem_id, &found_paper, 1);
    }

    if (ch == 'm') {
      struct sembuf found_match {
        MATCHES, 1, 0
      };
      semop(sem_id, &found_match, 1);
    }
    sleep(1);
  }

  sleep(5);

  struct sembuf barmen {
    BARMEN_SEM, 1, 0
  };
  semop(sem_id, &barmen, 1);
  kill(s1, SIGINT);
  kill(s2, SIGINT);
  kill(s3, SIGINT);
  exit(0);
}

void smoker(int id, int sem_id) {
  char items[3][10] = {"TABACCO", "MATCHES", "PAPER"};
  // unsigned short needed_item = rand() % 3;
  unsigned short needed_item = id;
  ;
  while (true) {
    struct sembuf barmen_wait {
      BARMEN_SEM, -1, 0
    };
    semop(sem_id, &barmen_wait, 1);
    // if(semctl(sem_id, needed_item, GETVAL, 0) > 0){

    // barmen give the item
    struct sembuf item_recived {
      needed_item, -1, 0
    };
    semop(sem_id, &item_recived, 1);

    std::cout << "smoking " << items[needed_item] << " by " << id << std::endl;
    sleep(rand() % 3);

    struct sembuf barmen_free {
      BARMEN_SEM, 1, 0
    };
    semop(sem_id, &barmen_free, 1);
    //}
  }
  exit(0);
}

int main() {

  // create semaphores
  sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0644);

  if (sem_id < 0) {
    perror("error creating semaphores");
    exit(errno);
  }

  if (semctl(sem_id, TABACCO, SETVAL, 0) < 0) {
    perror("tabacco semctl error");
    exit(errno);
  }
  if (semctl(sem_id, MATCHES, SETVAL, 0) < 0) {
    perror("matches semctl error");
    exit(errno);
  }
  if (semctl(sem_id, PAPER, SETVAL, 0) < 0) {
    perror("paper semctl error");
    exit(errno);
  }
  if (semctl(sem_id, BARMEN_SEM, SETVAL, 1) < 0) {
    perror("barmen semctl error");
    exit(errno);
  }

  // make processes
  int smokers_pid[3];

  for (int i = 0; i < 3; ++i) {

    smokers_pid[i] = fork();
    if (smokers_pid[i] < 0) {
      perror("fork error");
      exit(errno);
    }
    if (smokers_pid[i] == 0) {
      smoker(i, sem_id);
    }
  }

  int barmen_pid = fork();
  if (barmen_pid == 0) {
    barmen(sem_id, smokers_pid[0], smokers_pid[1], smokers_pid[2]);
  }

  wait(NULL);
  wait(NULL);
  wait(NULL);
  wait(NULL);
  return 0;
}

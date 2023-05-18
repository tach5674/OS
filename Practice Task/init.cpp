#include <fcntl.h>
#include "header.cpp"

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  creat("shm_file", 0600);
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, n * sizeof(BankAccount), 0600 | IPC_CREAT | IPC_EXCL);
  if (shmid == -1) {
    if (errno != EEXIST) {
      std::cout << "ERROR: Cannot create shared mem segment!\n";
      exit(1);
    } else {
      shmid = shmget(key, 0, 0600);
    }
  }

  BankAccount* bank = (BankAccount*)shmat(shmid, NULL, 0);
  if (bank == (BankAccount*)-1) {
    perror("Shared memory attach");
    exit(1);
  }

  for (int i = 0; i < n; ++i) {
      bank[i].current_balance = 0;
      bank[i].minimal_balance = 0;
      bank[i].maximal_balance = 10000;
      bank[i].is_frozen = false;
  }
  
  if (shmdt(bank) == -1) {
    perror("shmdt");
    exit(1);
  }

  Semaphore sem("sem_file");
}

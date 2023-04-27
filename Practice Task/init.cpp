#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
  int n = atoi(argv[1]);
  creat("shm_file", 0600);
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, (n * 4 + 1) * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
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

  int *bank = (int *)shmp;
  bank[0] = n * 4;
  for(int i = 1; i < n * 4; ++i){
    if(i % 4 == 3){
      bank[i] = 100;
      continue;
    }
    bank[i] = 0;
  }
  if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
  }
}


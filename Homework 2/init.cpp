#include <fcntl.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
  creat("shm_file", 0600);
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, 300 * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
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
    return 1;
  }
  int *arr = (int *)shmp;
  for (int i = 0; i < 300; ++i) {
    arr[i] = 0;
  }
  if (shmdt(shmp) == -1) {
    perror("shmdt");
    return 1;
  }
  return 0;
}

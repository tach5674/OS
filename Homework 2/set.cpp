#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, 300 * sizeof(int), IPC_EXCL);
  if (shmid == -1) {
    perror("Shared memory");
    exit(1);
  }
  void *shmp = shmat(shmid, NULL, 0);
  if (shmp == (void *)-1) {
    perror("Shared memory attach");
    return 1;
  }
  int *arr = (int *)shmp;
  for (int i = 2; i * i < 300; ++i) {
    for (int j = i * i; j < 300; j += i) {
      arr[j] = 1;
    }
  }
  if (shmdt(shmp) == -1) {
    perror("shmdt");
    return 1;
  }
  return 0;
}

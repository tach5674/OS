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
  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("shmctl");
    return 1;
  }
  return 0;
}

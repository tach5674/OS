#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define THREADS 7

pthread_mutex_t m;
pthread_cond_t printer_available;
int printer_working = 0;

void *print(void *arg) {
  int *id = (int *)arg;
  while (true) {
    pthread_mutex_lock(&m);
    while (printer_working == 1) {
      pthread_cond_wait(&printer_available, &m);
    }
    printer_working = 1;
    sleep(2);
    printf("%d\n", *id);
    printer_working = 0;
    pthread_cond_signal(&printer_available);
    pthread_mutex_unlock(&m);
    sleep(2);
  }

  return nullptr;
}

int main() {
  pthread_mutex_init(&m, NULL);
  pthread_cond_init(&printer_available, NULL);
  int indexes[THREADS];
  for (int i = 0; i < THREADS; ++i) {
    indexes[i] = i;
  }
  pthread_t tr[THREADS];
  for (int i = 0; i < THREADS; ++i) {
    pthread_create(&tr[i], NULL, print, &indexes[i]);
  }
  for (int i = 0; i < THREADS; ++i) {
    pthread_join(tr[i], NULL);
  }
  pthread_mutex_destroy(&m);
  pthread_cond_destroy(&printer_available);
  return 0;
}

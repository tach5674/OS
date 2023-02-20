#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>

#define COUNT 4

pthread_mutex_t m;

std::string src;
std::string dest;

void *copy_string(void *n) {
  int *arr = (int *)n;

  pthread_mutex_lock(&m);
  for (int i = arr[0]; i < arr[1]; ++i) {
    dest += src[i];
  }
  pthread_mutex_unlock(&m);
  return nullptr;
}

int main() {
  pthread_mutex_init(&m, NULL);
  std::cin >> src;
  int arr[COUNT][2];
  int step = src.length() / COUNT;
  int k = 0;
  for (int i = 0; i < 4; ++i) {
    arr[i][0] = k;
    k += step;
    arr[i][1] = k;
  }
  arr[3][1] = src.length();

  pthread_t tr[COUNT];

  for (int i = 0; i < COUNT; ++i) {
    pthread_create(&tr[i], NULL, copy_string, &arr[i]);
  }

  for (int i = 0; i < COUNT; ++i) {
    pthread_join(tr[i], NULL);
  }

  std::cout << dest << "\n";
  pthread_mutex_destroy(&m);

  return 0;
}

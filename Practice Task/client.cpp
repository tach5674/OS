#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main(){
  key_t key = ftok("shm_file", 1);
  int shmid = shmget(key, 0, 0);
  if (shmid == -1) {
    perror("Shared memory");
    exit(1);
  }
  void *shmp = shmat(shmid, NULL, 0);
  if (shmp == (void *)-1) {
    perror("Shared memory attach");
    exit(1);
  }
  
  int *bank = (int *)shmp;
  int op_num;
  
  while(1){
    std::cout << "\nEnter number of the operation you want to commit.\n";
    std::cout << "1: See the current balance of account.\n";
    std::cout << "2: See the minimal balance of account.\n";
    std::cout << "3: See the maximal balance of account.\n";
    std::cout << "4: Freeze an account.\n";
    std::cout << "6: Add a sum to all accounts.\n";
    std::cout << "10: Exit.\n";
    std::cin >> op_num;
    if(op_num < 1){
      std::cout << "invalid number. Please tryu again.";
    }
    if(op_num == 1){
      std::cout << "Enter account number: ";
      int acc_num;
      std::cin >> acc_num;
      std::cout << "\nCurrent balance of account: " << acc_num << " is " << bank[(acc_num-1) * 4 + 1] << "\n"; 
    }
    if(op_num == 2){
      std::cout << "Enter account number: ";
      int acc_num;
      std::cin >> acc_num;
      std::cout << "\nMinimal balance of account: " << acc_num << " is " << bank[(acc_num-1) * 4 + 2] << "\n"; 
    }
    if(op_num == 3){
      std::cout << "Enter account number: ";
      int acc_num;
      std::cin >> acc_num;
      std::cout << "\nMaximal balance of account: " << acc_num << " is " << bank[(acc_num-1) * 4 + 3] << "\n"; 
    }
    if(op_num == 4){
      std::cout << "Enter account number: ";
      int acc_num;
      std::cin >> acc_num;
      if(bank[(acc_num-1) * 4 + 4] == 1){
        std::cout << "\nAccount " << acc_num << " is already frozen";
        continue;
      }
      bank[(acc_num-1) * 4 + 4] += 1; 
      std::cout << "\nAccount " << acc_num << " has been frozen.\n"; 
    }
    if(op_num == 6){
      bool is_frozen = false;
      for(int i = 4; i < bank[0]; i += 4){
        if(bank[i] == 1){
          is_frozen = true;
          break;
        }
      }
      if(is_frozen == true){
        std::cout << "\nYou have frozen accounts.\n";
        continue;
      }
      std::cout << "Enter the sum you want to add: ";
      int sum;
      std::cin >> sum;
      for(int i = 1; i < bank[0]; i += 4){
        bank[i] += sum;
      }
      std::cout << "\n" << sum << " was added to all accounts.\n"; 
    }
    if(op_num == 10){
      break;
    }
  }
  if (shmdt(shmp) == -1) {
      perror("shmdt");
      exit(1);
  }
  return 0;
}

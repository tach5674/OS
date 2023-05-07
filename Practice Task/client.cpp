#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <vector>

class BankAccount {
public:
  int *current;
  int *minimal;
  int *maximal;
  int *frozen;

  BankAccount(int &cur, int &min, int &max, int &frz) {
    current = &cur;
    minimal = &min;
    maximal = &max;
    frozen = &frz;
  }
};

int main() {
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
  int acc_num;

  std::vector<BankAccount *> Bank(bank[0] + 1);
  
  int account_count = bank[0];
  for (int i = 1; i < Bank.size(); ++i) {
    Bank[i] = new BankAccount(bank[(i - 1) * 4 + 1], bank[(i - 1) * 4 + 2],
                              bank[(i - 1) * 4 + 3], bank[(i - 1) * 4 + 4]);
  }

  while (1) {
    std::cout << "\nEnter number of the operation you want to commit:\n\n";
    std::cout << "1: See the current/minimal/maximal balance of account.\n";
    std::cout << "2: Freeze/unfreeze an account.\n";
    std::cout << "3: Add chosen amount to all accounts.\n";
    std::cout << "4: Withdraw chosen amount from all accounts.\n";
    std::cout << "5: Transfer chosen amount of from one account to another.\n";
    std::cout << "6: Set minimal/maximal amount you can have on chosen account.\n";
    std::cout << "7: Exit.\n";
    std::cout << "\nInput: ";

    std::cin >> op_num;
    if (op_num < 1 || op_num > 7) {
      std::cout << "Invalid operation number, please try again.";
      continue;
    }
    if (op_num == 1) {
      while(1){
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        if(acc_num < 1 || acc_num > account_count){
          std::cout << "\nInvalid account number, Please try again.\n";
          continue;
        }
        break;
      }
      std::cout << "\nCurrent balance of account: " << acc_num << " is "
                << *(Bank[acc_num]->current) << "\n";
      std::cout << "\nMinimal balance of account: " << acc_num << " is "
                << *(Bank[acc_num]->minimal) << "\n";
      std::cout << "\nMaximal balance of account: " << acc_num << " is "
                << *(Bank[acc_num]->maximal) << "\n";
      continue;
    }
    if (op_num == 2) {
      while(1){
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        if(acc_num < 1 || acc_num > account_count){
          std::cout << "\nInvalid account number, Please try again.\n";
          continue;
        }
        break;
      }
      if (*(Bank[acc_num]->frozen) == 1) {
        std::cout << "\nAccount " << acc_num << " is frozen.\n";
       
        while(1){ 
          std::cout << "\n1: Unfreeze account " << acc_num << "\n";
          std::cout << "2: Exit.\n";
          std::cout << "\nInput: ";
          std::cin >> op_num;
          if (op_num < 1 || op_num > 2) {
            std::cout << "Invalid operation number, please try again.";
            continue;
          }
          break;
        }
        if (op_num == 1) {
          *(Bank[acc_num]->frozen) = 0;
          std::cout << "\nAccount " << acc_num << " has been unfrozen.\n";
          continue;
        }
        if (op_num == 2) {
          continue;
        }
      } 
      else {
        std::cout << "\nAccount " << acc_num << " is unfrozen.\n";
        while(1){ 
          std::cout << "\n1: Freeze account " << acc_num << "\n";
          std::cout << "2: Exit.\n";
          std::cout << "\nInput: ";
          std::cin >> op_num;
          if (op_num < 1 || op_num > 2) {
            std::cout << "\n\nInvalid operation number, please try again.\n";
            continue;
          }
          break;
        }
        if (op_num == 1) {
          *(Bank[acc_num]->frozen) = 1;
          std::cout << "\nAccount " << acc_num << " has been frozen.\n";
          continue;
        }
        if (op_num == 2) {
          continue;
        }
      }
    }
    if (op_num == 3) {
      int sum;
      bool is_frozen = false;
      bool not_in_limits = false;
      
      while(1){
        std::cout << "\nEnter the sum you want to add: ";
        std::cin >> sum;
        if(sum < 1){
          std::cout << "\nAdding sum should be positive, please try again.\n";
          continue;
        }
        for (int i = 1; i < Bank.size(); ++i) {
          if (*(Bank[i]->frozen) == 1) {
            is_frozen = true;
            break;
          }
          if ((*(Bank[i]->current) + sum) > *Bank[i]->maximal) {
            not_in_limits = true;
            break;
          }
        }
        if (is_frozen == true) {
          std::cout << "\nYou have frozen accounts!\n";
          break;
        }
        if (not_in_limits == true) {
          std::cout << "\nYour balance will run over maximal!";
          while(1){ 
            std::cout << "\n1:Try another sum.";
            std::cout << "\n2:Exit.\n";
            std::cout << "\nInput: ";
            std::cin >> op_num;
            if (op_num < 1 || op_num > 2) {
              std::cout << "\n\nInvalid operation number, please try again.\n";
              continue;
            }
            break;
          }
          if (op_num == 1) {
            continue;
          }
        }
        break;
      }
      if (is_frozen == true) {
        continue;
      }
      if(op_num == 2){
        continue;
      }

      for (int i = 1; i < Bank.size(); ++i) {
        *Bank[i]->current += sum;
      }
      std::cout << "\n" << sum << " was added to all accounts.\n";
      continue;
    }
    if (op_num == 4) {
      int sum;
      bool is_frozen = false;
      bool not_in_limits = false;
      
      while(1){
        std::cout << "\nEnter the sum you want to withdraw: ";
        std::cin >> sum;
        if(sum < 1){
          std::cout << "\nWithdrawing sum should be positive, please try again.\n";
          continue;
        }
        for (int i = 1; i < Bank.size(); ++i) {
          if (*(Bank[i]->frozen) == 1) {
            is_frozen = true;
            break;
          }
          if ((*(Bank[i]->current) - sum) < *Bank[i]->minimal) {
            not_in_limits = true;
            break;
          }
        }
        if (is_frozen == true) {
          std::cout << "\nYou have frozen accounts!\n";
          break;
        }
        if (not_in_limits == true) {
          std::cout << "\nYour balance will run under minimal!.\n";
          while(1){ 
            std::cout << "\n1:Try another sum.";
            std::cout << "\n2:Exit.\n";
            std::cout << "\nInput: ";
            std::cin >> op_num;
            if (op_num < 1 || op_num > 2) {
              std::cout << "\n\nInvalid operation number, please try again.\n";
              continue;
            }
            break;
          }
          if (op_num == 1) {
            continue;
          }
        }
        break;
      } 
      if (is_frozen == true) {
        continue;
      }
      if(op_num == 2){
        continue;
      }

      for (int i = 1; i < Bank.size(); ++i) {
        *Bank[i]->current -= sum;
      }
      std::cout << "\n" << sum << " was withdrawn from all accounts.\n";
      continue;
    }
    if (op_num == 5) {
      int acc_from;
      while(1){
        std::cout << "\nEnter the account number you want to transfer from: ";
        std::cin >> acc_from;
        if(acc_from < 1 || acc_from > account_count){
          std::cout << "\nInvalid account number, Please try again.\n";
          continue;
        }
        break;
      }
      int acc_to;
      while(1){
        std::cout << "Enter the account number you want to transfer to: ";
        std::cin >> acc_to;
        if(acc_to < 1 || acc_to > account_count){
          std::cout << "\nInvalid account number, Please try again.\n";
          continue;
        }
        break;
      }
      int sum;
      bool is_frozen = false;
      while(1){
        std::cout << "\nEnter the sum you want to transfer: ";
        std::cin >> sum;
        if(sum < 1){
          std::cout << "\nTransfering sum should be positive, please try again.\n";
          continue;
        }
        if (*(Bank[acc_from]->frozen) == 1) {
          std::cout << "\nAccount you want to transfer from is frozen!\n";
          is_frozen = true;
          break;
        }
        if (*(Bank[acc_to]->frozen) == 1) {
          std::cout << "\nAccount you want to transfer to is frozen!\n";
          is_frozen = true;
          continue;
        }
        if ((*(Bank[acc_from]->current) - sum) < *Bank[acc_from]->minimal) {
          std::cout << "Account " << acc_from << " balance will run under minimal!\n";
          while(1){ 
            std::cout << "\n1:Try another sum.";
            std::cout << "\n2:Exit.\n";
            std::cout << "\nInput: ";
            std::cin >> op_num;
            if (op_num < 1 || op_num > 2) {
              std::cout << "\n\nInvalid operation number, please try again.\n";
              continue;
            }
            break;
          }
          if (op_num == 1) {
            continue;
          }
          break;
        }
        if ((*(Bank[acc_to]->current) + sum) > *Bank[acc_to]->maximal) {
          std::cout << "Account " << acc_to << "\n balance will run over maximal!";
          while(1){ 
            std::cout << "\n1:Try another sum.";
            std::cout << "\n2:Exit.\n";
            std::cout << "\nInput: ";
            std::cin >> op_num;
            if (op_num < 1 || op_num > 2) {
              std::cout << "\n\nInvalid operation number, please try again.\n";
              continue;
            }
            break;
          }
          if (op_num == 1) {
            continue;
          }
          break;
        }  
        break;
      }
      if(is_frozen == true){
        continue;
      }
      if (op_num == 2) {
            continue;
      }
      *Bank[acc_from]->current -= sum;
      *Bank[acc_to]->current += sum;
      std::cout << "\n" << sum << " was trasfered from account " << acc_from << " to account " << acc_to << ".\n";
      continue;
    }
    if (op_num == 6) {
      while(1){
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        if(acc_num < 1 || acc_num > account_count){
          std::cout << "\nInvalid account number, Please try again.\n";
          continue;
        }
        break;
      }
      int amount;
      while(1){ 
        std::cout << "\n1: Change minimal amount.";
        std::cout << "\n2: Change maximal amount.\n";
        std::cout << "\n2: Input: ";
        std::cin >> op_num;
        if (op_num < 1 || op_num > 2) {
          std::cout << "\n\nInvalid operation number, please try again.\n";
          continue;
        }
        break;
      }
      if(op_num == 1){
        op_num = 0;
        while(1){
          std::cout << "\nEnter new minimal amount: ";
          std::cin >> amount;
          if(*(Bank[acc_num]->current) < amount){
            std::cout << "\nYour current amount will be less than new minimal!\n";
            while(1){
              std::cout << "\n1: Try another amount.";
              std::cout << "\n2: Exit\n";
              std::cout << "\nInput: ";
              std::cin >> op_num;
              if (op_num < 1 || op_num > 2) {
                std::cout << "\n\nInvalid operation number, please try again.\n";
                continue;
              }
              break;
            }
            if(op_num == 1){
              continue;
            }
          }
          break;
        }
        if(op_num == 2){
          continue;
        }
        
        *(Bank[acc_num]->minimal) = amount;
        std::cout << "\nAccount " << acc_num << " minimal amount was set to " << amount << "\n";  
        continue;
      }
      if(op_num == 2){
        op_num = 0;
        while(1){
          std::cout << "\nEnter new maximal amount: ";
          std::cin >> amount;
          if(*(Bank[acc_num]->current) > amount){
            std::cout << "\nYour current amount will be more than new maximal!\n";
            while(1){
              std::cout << "\n1: Try another amount.";
              std::cout << "\n2: Exit\n";
              std::cout << "\nInput: ";
              std::cin >> op_num;
              if (op_num < 1 || op_num > 2) {
                std::cout << "\n\nInvalid operation number, please try again.\n";
                continue;
              }
              break;
            }
            if(op_num == 1){
              continue;
            }
          }
          break;
        }
        if(op_num == 2){
          continue;
        }
        
        *(Bank[acc_num]->maximal) = amount;
        std::cout << "\nAccount " << acc_num << " maximal amount was set to " << amount << "\n";  
        continue;
      }
      continue;
    }
    if (op_num == 7) {
      break;
    }
  }
  
  if (shmdt(shmp) == -1) {
    perror("shmdt");
    exit(1);
  }
  return 0;
}

#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string>

class Semaphore {
private:
  int semid;
public:
  Semaphore(std::string name_key){
    key_t key = ftok(name_key.c_str(), 1);
    if ((this->semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
      perror("semget");
      exit(1);
    }
    if (semctl(semid, 0, SETVAL, 1) == -1) {
      perror("semctl");
    }
  }

  void SemWait(){
    struct sembuf semops{0, -1, 0};
    if (semop(this->semid, &semops, 1) == -1) {
      perror("semop");
    }
  }

  void SemPost(){
    struct sembuf semops{0, 1, 0};
    if (semop(this->semid, &semops, 1) == -1) {
      perror("semop");
    }
  }
  void Destroy(){
    if (semctl(this->semid, 0, IPC_RMID, NULL) == -1) {
      perror("semctl");
    }
  }
};

class BankAccount {
public:
  int current_balance;
  int minimal_balance;
  int maximal_balance;
  bool is_frozen;
};

class Bank {
    BankAccount* accounts;
    int account_count;
    Semaphore *sem;
public:
    Bank(){
        key_t key = ftok("shm_file", 1);
        int shmid = shmget(key, 0, 0);
        if (shmid == -1) {
            perror("Shared memory");
            exit(1);
        }
        this->accounts = (BankAccount*)shmat(shmid, NULL, 0);
        if (this->accounts == (BankAccount*)-1) {
            perror("Shared memory attach");
            exit(1);
        }
        struct shmid_ds buf;
        shmctl(shmid, IPC_STAT, &buf);
        this->account_count = (int) buf.shm_segsz / sizeof(BankAccount);

        this->sem = new Semaphore("sem_file");
    }

    ~Bank(){
        if (shmdt(accounts) == -1) {
            perror("shmdt");
            exit(1);
        } 
        delete sem;
    }

    void DisplayBalance(int acc_num){
      sem->SemWait();
      if(acc_num < 1 || acc_num > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      std::cout << "\nCurrent balance of account: " << acc_num << " is "
              << this->accounts[acc_num - 1].current_balance << "\n";
      std::cout << "\nMinimal balance of account: " << acc_num << " is "
              << this->accounts[acc_num - 1].minimal_balance << "\n";
      std::cout << "\nMaximal balance of account: " << acc_num << " is "
              << this->accounts[acc_num - 1].maximal_balance << "\n";
      sem->SemPost();
    }
    void Freeze(int acc_num){
      sem->SemWait();
      if(acc_num < 1 || acc_num > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if (this->accounts[acc_num - 1].is_frozen == true) {
        std::cout << "\nAccount " << acc_num << " is already frozen.\n";
        sem->SemPost();
        return;
      }
      this->accounts[acc_num].is_frozen = true;
      std::cout << "\nAccount " << acc_num << " has been frozen.\n";
      sem->SemPost();
    }
    void Unfreeze(int acc_num){
      sem->SemWait();
      if(acc_num < 1 || acc_num > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if (this->accounts[acc_num - 1].is_frozen == false) {
        std::cout << "\nAccount " << acc_num << " is already unfrozen.\n";
        sem->SemPost();
        return;
      }
      this->accounts[acc_num - 1].is_frozen = false;
      std::cout << "\nAccount " << acc_num << " has been unfrozen.\n";
      sem->SemPost();
    }
    void SetMinimalBalance(int acc_num, int amount){
      sem->SemWait();
      if(acc_num < 1 || acc_num > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if(this->accounts[acc_num - 1].current_balance < amount){
        std::cout << "\nMinimal balance will run over current!\n";
        sem->SemPost();
        return;
      }
      this->accounts[acc_num - 1].minimal_balance = amount;
      std::cout << "\nMinimal balance was set to " << amount << "\n";
      sem->SemPost();
    }
    void SetMaximalBalance(int acc_num, int amount){
      sem->SemWait();
      if(acc_num < 1 || acc_num > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if(this->accounts[acc_num - 1].current_balance > amount){
        std::cout << "\nMaximal balance will run under current!\n";
        sem->SemPost();
        return;
      }
      this->accounts[acc_num - 1].maximal_balance = amount;
      std::cout << "\nMaximal balance was set to " << amount << "\n";
      sem->SemPost();
    }
    void AddSumToAllAccounts(int sum){
      sem->SemWait();
      for(int i = 0; i < this->account_count; ++i){
        if(accounts[i].is_frozen == true){
          std::cout <<"\nYou have frozen accounts!\n";
          sem->SemPost();
          return;
        }
        else if(accounts[i].maximal_balance < accounts[i].current_balance + sum){
          std::cout <<"\nOne of accounts maximal balance will run under current!\n";
          sem->SemPost();
          return;
        }
      }
      for(int i = 0; i < this->account_count; ++i){
        accounts[i].current_balance += sum;
      }
      std::cout << "\n" << sum <<" was added to all accounts.\n";
      sem->SemPost();
    }
    void WithdrawSumFromAllAccounts(int sum){
      sem->SemWait();
      for(int i = 0; i < this->account_count; ++i){
        if(accounts[i].is_frozen == true){
          std::cout <<"\nYou have frozen accounts!\n";
          sem->SemPost();
          return;
        }
        else if(accounts[i].minimal_balance > accounts[i].current_balance - sum){
          std::cout <<"\nOne of accounts minimal balance will run over current!\n";
          sem->SemPost();
          return;
        }
      }
      for(int i = 0; i < this->account_count; ++i){
        accounts[i].current_balance -= sum;
      }
      std::cout << "\n" << sum <<" was withdrawn from all accounts.\n";
      sem->SemPost();
    }
    void Transfer(int acc_from, int acc_to, int amount){
      sem->SemWait();
      if(acc_from < 1 || acc_from > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if(acc_to < 1 || acc_to > this->account_count){
        std::cout << "\nInvalid account number, Please try again.\n";
        sem->SemPost();
        return;
      }
      if(this->accounts[acc_from - 1].minimal_balance > this->accounts[acc_from - 1].current_balance - amount){
        std::cout << "\nMinimal balance will run over current!\n";
        sem->SemPost();
        return;
      }
      if(this->accounts[acc_to - 1].maximal_balance < this->accounts[acc_to - 1].current_balance + amount){
        std::cout << "\nMAximal balance will run under current!\n";
        sem->SemPost();
        return;
      }
      this->accounts[acc_from - 1].current_balance -= amount;
      this->accounts[acc_to - 1].current_balance += amount;
      std::cout << "\n" << amount << " was trasfered from account " << acc_from << " to account " << acc_to << ".\n";
      sem->SemPost();
    }
};


#endif

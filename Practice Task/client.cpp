#include <sys/wait.h>
#include <vector>
#include "header.cpp"

int main() {
  Bank bank;
  
  int op_num;
  int acc_num;
  int amount;
  
  while (1) {
    std::cout << "\nEnter number of the operation you want to commit:\n\n";
    std::cout << "1: See the current/minimal/maximal balance of account.\n";
    std::cout << "2: Freeze an account.\n";
    std::cout << "3: Unfreeze an account.\n";
    std::cout << "4: Set minimal amount you can have on chosen account.\n";
    std::cout << "5: Set maximal amount you can have on chosen account.\n";
    std::cout << "6: Add chosen amount to all accounts.\n";
    std::cout << "7: Withdraw chosen amount from all accounts.\n";
    std::cout << "8: Transfer chosen amount of from one account to another.\n";
    std::cout << "9: Exit.\n";
    std::cout << "\nInput: ";

    std::cin >> op_num;
    if (op_num < 1 || op_num > 9) {
      std::cout << "Invalid operation number, please try again.";
      continue;
    }
    if (op_num == 1) {
      std::cout << "\nEnter account number: ";
      std::cin >> acc_num;
      bank.DisplayBalance(acc_num); 
    }
    if (op_num == 2) {
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        bank.Freeze(acc_num);
    }
    if (op_num == 3) {
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        bank.Unfreeze(acc_num);
    }
    if (op_num == 4) {
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        std::cout << "\nEnter new minimal amount: ";
        std::cin >> amount;
        bank.SetMinimalBalance(acc_num, amount);
    }
    if (op_num == 5) {
        std::cout << "\nEnter account number: ";
        std::cin >> acc_num;
        std::cout << "\nEnter new maximal amount: ";
        std::cin >> amount;
        bank.SetMaximalBalance(acc_num, amount);
    }
    if (op_num == 6) {
        std::cout << "\nEnter the sum you want to add: ";
        std::cin >> amount;
        bank.AddSumToAllAccounts(amount);
    }
    if (op_num == 7) {
        std::cout << "\nEnter the sum you want to withdraw: ";
        std::cin >> amount;
        bank.WithdrawSumFromAllAccounts(amount);
    }
    if (op_num == 8) {
        int acc_from;
        int acc_to;
        std::cout << "\nEnter account number you want to transfer from: ";
        std::cin >> acc_from;
        std::cout << "\nEnter account number you want to transfer to: ";
        std::cin >> acc_to;
        std::cout << "\nEnter the sum you want to transfer: ";
        std::cin >> amount;
        bank.Transfer(acc_from, acc_to, amount);
    }
    if(op_num == 9){
      break;
    }
  }
  return 0;
}

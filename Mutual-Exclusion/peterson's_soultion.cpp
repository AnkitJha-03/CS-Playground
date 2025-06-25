#include <iostream>
#include <thread>

int turn = 0;
bool interested[2];
int x = 0;

void enter_section(int process) {
  int other = 1 - process;
  interested[process] = true;
  turn = process;
  // if its my turn, I will wait until the other process is not interested
  while (turn == process && interested[other] == true);
}

void leave_section(int process) {
  interested[process] = false;
}

class thread_object {
 public:
  void operator()(int process) {
    for (int i = 0; i < 100; i++) {
      enter_section(process);
      x += 1;
      std::cout << "Thread " << process << " " << x << std::endl;
      leave_section(process);
    }
  }
};

int main() {
  thread_object obj;
  std::thread th1(obj, 0);
  std::thread th2(obj, 1);

  th1.join();
  th2.join();

  return 0;
}
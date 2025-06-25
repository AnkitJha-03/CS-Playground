#include <windows.h>

#include <iostream>
#include <thread>

int turn = 0;

void process0() {
  while (true) {
    while (turn != 0);  // Busy-wait (spin), until it's Process 0's turn

    // Critical section for Process 0
    std::cout << "Process 0 in critical section" << std::endl;
    Sleep(500);
    turn = 1;  // Release lock (give turn to Process 1)

    // non-critical section for Process 0
    Sleep(500);
  }
}

void process1() {
  while (true) {
    while (turn != 1);  // Busy-wait (spin), until it's Process 1's turn

    // Critical section for Process 1
    std::cout << "Process 1 in critical section" << std::endl;
    Sleep(500);
    turn = 0;  // Release lock (give turn to Process 0)

    // non-critical section for Process 1
    Sleep(5000);
  }
}

int main() {
  std::thread t0(process0);
  std::thread t1(process1);

  t0.join();
  t1.join();

  return 0;
}
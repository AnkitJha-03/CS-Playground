#include <windows.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// THIS IS WRONG IMPLEMENTATION OF PRODUCER-CONSUMER PROBLEM

const int N = 5;  // Buffer size
int count = 0;    // Shared variable (unsafe)

// Condition variable and mutex for conditional wake-up
std::condition_variable cv;
std::mutex mtx;

void producer() {
  while (true) {
    if (count == N) {
      std::unique_lock<std::mutex> lock(mtx);
      std::cout << "Producer sleeping: Buffer filled" << std::endl;
      cv.wait(lock);
    } else {
      count++;
      std::cout << "Produced, count = " << count << std::endl;

      // Wake consumer if it might be sleeping
      if (count == 1) {
        cv.notify_one();
      }
    }
    // Sleep(1000); // Simulate work
  }
}

void consumer() {
  while (true) {
    if (count == 0) {
      std::unique_lock<std::mutex> lock(mtx);
      std::cout << "Consumer sleeping: (count == 0)" << std::endl;
      cv.wait(lock);
    } else {
      count--;  // Unsafe decrement
      std::cout << "Consumed, count = " << count << std::endl;

      // Wake producer if it might be sleeping
      if (count == N - 1) {
        cv.notify_one();
      }
    }
    // Sleep(1000); // Simulate work
  }
}

int main() {
  std::thread t1(producer);
  std::thread t2(consumer);
  // std::thread t3(producer);
  // std::thread t4(consumer);

  t1.join();
  t2.join();
  // t3.join();
  // t4.join();

  return 0;
}

// problem arises :
// one of the thread read count variable and context switch happens,
// to another thread, it either filled or empty buffer,
// and the second thread won't be aware of the change in count variable as it
// had already read it, so it will never wake up the first thread.

// solution 1 :
// use mutex, such that either one of the thread (producer or consumer) can run
// at a time, limitation : only one thread running at a time. implementation :
// lock at start of both functions, for multiple producers and consumers we have
// to notify all the threads.
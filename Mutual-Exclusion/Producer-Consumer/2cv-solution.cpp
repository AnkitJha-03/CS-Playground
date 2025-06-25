#include <windows.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

const int N = 5;  // Buffer size
int count = 0;    // Shared variable

std::mutex mtx;
std::condition_variable cv_producer;
std::condition_variable cv_consumer;
std::mutex cout_mtx;  // Just for output synchronization

void print_message(const std::string &action, int value) {
  std::lock_guard<std::mutex> lock(cout_mtx);
  std::cout << "[" << std::this_thread::get_id() << "] " << action
            << " (count=" << value << ")\n";
}

void producer() {
  while (true) {
    int new_count;
    // PHASE 1: Reserve space (serialized)
    {
      std::unique_lock<std::mutex> lock(mtx);
      cv_producer.wait(lock, [&]() { return count < N; });
      new_count = ++count;
      print_message("RESERVED PRODUCE ITEM AT", new_count - 1);
    }

    // PHASE 2: Actual production (parallel)
    Sleep(500);  // Simulate work
    print_message("COMPLETED PRODUCTION AT", new_count - 1);

    // PHASE 3: Notify if buffer was empty
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (new_count == 1) cv_consumer.notify_all();
    }
  }
}

void consumer() {
  while (true) {
    int new_count;
    // PHASE 1: Reserve item (serialized)
    {
      std::unique_lock<std::mutex> lock(mtx);
      cv_consumer.wait(lock, [&]() { return count > 0; });
      new_count = --count;
      print_message("RESERVED CONSUME ITEM AT", new_count + 1);
    }

    // PHASE 2: Actual consumption (parallel)
    Sleep(1000);  // Simulate work
    print_message("COMPLETED CONSUMPTION AT", new_count + 1);

    // PHASE 3: Notify if buffer was full
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (new_count == N - 1) cv_producer.notify_all();
    }
  }
}

int main() {
  std::thread p1(producer), p2(producer);
  std::thread c1(consumer), c2(consumer);

  p1.join();
  p2.join();
  c1.join();
  c2.join();

  return 0;
}

// In this solution:
// 1. The producer and consumer threads are separated into three phases:
//    - Phase 1: Reserve space or item (serialized with mutex).
//    - Phase 2: Perform the actual work (parallel).
//    - Phase 3: Notify the other party if necessary (serialized with mutex).
// 2. Multiple producer and consumer threads are running concurrently.
// 3. limitation: only producer that read the count variable = 0 can notify the
// consumer, and vice versa
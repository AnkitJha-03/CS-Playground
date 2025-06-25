#include <windows.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class Semaphore {
  std::mutex mtx;
  std::condition_variable cv;
  int count;

 public:
  Semaphore(int c = 0) : count(c) {
  }
  void wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&] { return count > 0; });
    count--;
  }
  void signal() {
    std::lock_guard<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
  }
};

std::queue<int> buffer;
std::mutex print_mtx;
Semaphore empty(5), full(0);

void producer(int id) {
  int item = 0;
  while (true) {
    empty.wait();
    {
      std::lock_guard<std::mutex> lock(print_mtx);
      buffer.push(++item);
      std::cout << "P" << id << " made " << item << " | Size: " << buffer.size()
                << "\n";
    }
    Sleep(500);  // actual production time
    full.signal();
  }
}

void consumer(int id) {
  while (true) {
    full.wait();
    {
      std::lock_guard<std::mutex> lock(print_mtx);
      int item = buffer.front();
      buffer.pop();
      std::cout << "C" << id << " took " << item << " | Size: " << buffer.size()
                << "\n";
    }
    Sleep(500);  // actual consumption time
    empty.signal();
  }
}

int main() {
  std::thread p1(producer, 1), p2(producer, 2);
  std::thread c1(consumer, 1), c2(consumer, 2);

  p1.join();
  p2.join();
  c1.join();
  c2.join();

  return 0;
}
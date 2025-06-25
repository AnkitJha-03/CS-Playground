#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class Semaphore {
 private:
  int count;
  std::mutex mtx;
  std::condition_variable cv;

 public:
  Semaphore(int n = 0) : count(n) {
  }
  void set(int n) {
    count = n;
  }
  void signal() {
    std::unique_lock<std::mutex> lck(mtx);
    count++;
    cv.notify_one();
  }
  void wait() {
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [&]() { return count > 0; });
    count--;
  }
};

class DiningPhilosophers {
 private:
  std::mutex mtx[5];
  Semaphore sem;

 public:
  DiningPhilosophers() {
    sem.set(4);
  }

  void wantsToEat(int philosopher, void pickLeftFork(), void pickRightFork(),
                  void eat(), void putLeftFork(), void putRightFork()) {
    int left = philosopher, right = (philosopher + 1) % 5;

    sem.wait();  // At most 4 philosophers can "enter" in eat state

    std::unique_lock<std::mutex> lckLeft(mtx[left]);
    std::unique_lock<std::mutex> lckRight(mtx[right]);

    pickLeftFork(), pickRightFork();
    eat();
    putLeftFork(), putRightFork();

    lckLeft.unlock();
    lckRight.unlock();

    sem.signal();  // finish eating, release 1 spot
  }
};

void pickLeftFork();
void pickRightFork();
void eat();
void putLeftFork();
void putRightFork();
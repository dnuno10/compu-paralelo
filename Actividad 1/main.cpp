#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <iostream>
#include <print>
#include <mutex>
#include <condition_variable>
#include <chrono>

struct Fork {
    bool isTaken = false;
    std::string philosopher;
    std::mutex forkMutex;
    std::condition_variable taken;

    void takeFork(std::string_view philosopherName, std::string_view hand) {
        std::unique_lock<std::mutex> lock(forkMutex);
        std::println("{} is waiting for {} hand fork", philosopherName, hand);
        taken.wait(lock, [&]{ return !isTaken; });
        isTaken = true;
        philosopher = std::string(philosopherName);
        std::println("{} is taking {} hand fork", philosopher, hand);
    }

    bool tryTake(std::string_view philosopherName, std::string_view hand) {
        std::unique_lock<std::mutex> lock(forkMutex);
        if (isTaken) return false;
        isTaken = true;
        philosopher = std::string(philosopherName);
        std::println("{} is taking {} hand fork (try)", philosopher, hand);
        return true;
    }

    void releaseFork(std::string_view hand) {
        {
            std::lock_guard<std::mutex> lock(forkMutex);
            std::println("{} is releasing {} hand fork", philosopher, hand);
            isTaken = false;
            philosopher.clear();
        }
        taken.notify_all();
    }
};

class Philosopher {
private:
    std::string name;
    std::shared_ptr<Philosopher> rightPhilosopher = nullptr;
    std::shared_ptr<Fork> leftFork;
    std::shared_ptr<Fork> rightFork;
    
public:
    Philosopher(std::string name) : name(name) {}

    //get
    std::shared_ptr<Fork> RightFork() const {
        return rightFork;
    }
    //set
    std::shared_ptr<Fork> &RightFork() {
        return rightFork;
    }
     //get 
    std::shared_ptr<Fork> LeftFork() const {
        return leftFork;
    }
    //set
    std::shared_ptr<Fork> &LeftFork() {
        return leftFork;
    }

    void eat() {
        while (true) {
            leftFork->takeFork(name, "left");
            
            if (!rightFork->tryTake(name, "right")) {
                leftFork->releaseFork("left");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            std::println("{} started eating", name);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            rightFork->releaseFork("right");
            leftFork->releaseFork("left");
        }
    }
};

int main(){
    std::shared_ptr<Fork> fork1 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork2 = std::make_shared<Fork>();

    Philosopher socrates("Socrates");
    Philosopher plato("Plato");
    socrates.RightFork() = fork2;
    socrates.LeftFork() = fork1;
    plato.RightFork() = fork1;
    plato.LeftFork() = fork2;
    
    std::thread threadSocrates([&](){socrates.eat();});
    std::thread threadPlato([&](){plato.eat();});
    threadSocrates.join();
    threadPlato.join();
    return 0;
}
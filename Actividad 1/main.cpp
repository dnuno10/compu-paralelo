#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

struct Fork
{
    bool occupied = false;
    std::mutex mutex;
    std::condition_variable taken;

    void pickup(std::string_view philosopherName, std::string_view hand)
    {
        {
            std::unique_lock lock(mutex);
            taken.wait(lock, [&]
                       {
            std::cout << philosopherName << " is waiting for " << hand << " hand fork" << std::endl;
            return !occupied; });
            std::cout << philosopherName << " is taking " << hand << " hand fork" << std::endl;

            occupied = true;
        }
    }

    void layDown(std::string_view philosopherName, std::string_view hand)
    {
        std::cout << philosopherName << " is releasing " << hand << " hand fork" << std::endl;
        occupied = false;
        taken.notify_all();
    }
};

class Philosopher
{
private:
    std::string name;

    std::shared_ptr<Fork> rightFork;
    std::shared_ptr<Fork> leftFork;

public:
    Philosopher(std::string name) : name(name) {}

    std::shared_ptr<Fork> RightFork() const
    {
        return rightFork;
    }
    std::shared_ptr<Fork> &RightFork()
    {
        return rightFork;
    }

    std::shared_ptr<Fork> LeftFork() const
    {
        return leftFork;
    }
    std::shared_ptr<Fork> &LeftFork()
    {
        return leftFork;
    }

    void eat()
    {
        while (true)
        {
            leftFork->pickup(name, "left");
            if (rightFork->occupied)
            {
                leftFork->layDown(name, "left");
                continue;
            }
            rightFork->pickup(name, "right");

            // Simulating the Philosopher eating
            std::cout << name << " started eating" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << name << " is done eating" << std::endl;

            leftFork->layDown(name, "left");

            rightFork->layDown(name, "right");
        }
    }
};

int main()
{
    std::shared_ptr<Fork> fork1 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork2 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork3 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork4 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork5 = std::make_shared<Fork>();

    Philosopher socrates("Socrates");
    socrates.LeftFork() = fork1;
    socrates.RightFork() = fork2;

    Philosopher diogenes("Diogenes");
    diogenes.LeftFork() = fork2;
    diogenes.RightFork() = fork3;

    Philosopher platon("Platon");
    platon.LeftFork() = fork3;
    platon.RightFork() = fork4;

    // Cambié Phytagoras por Aristotle
    Philosopher aristotle("Aristotle");
    aristotle.LeftFork() = fork4;
    aristotle.RightFork() = fork5;

    // Cambié Zeno por Epicurus
    Philosopher epicurus("Epicurus");
    epicurus.LeftFork() = fork5;
    epicurus.RightFork() = fork1;

    std::thread threadSocrates([&]()
                               { socrates.eat(); });
    std::thread threadDiogenes([&]()
                               { diogenes.eat(); });
    std::thread threadPlaton([&]()
                             { platon.eat(); });
    std::thread threadAristotle([&]()
                                { aristotle.eat(); });
    std::thread threadEpicurus([&]()
                               { epicurus.eat(); });

    threadSocrates.join();
    threadDiogenes.join();
    threadPlaton.join();
    threadAristotle.join();
    threadEpicurus.join();

    return 0;
}
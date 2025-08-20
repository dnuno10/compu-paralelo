#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>

// Ejecuta 10 threads, cada uno suma 100 números aleatorios entre 1 y 1000.
// Muestra el resultado de cada thread y el de mayor puntuación.

class Thread
{
    int id_;
    int total_ = 0;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist{1, 1000};

public:
    explicit Thread(int id)
        : id_(id), rng(std::random_device{}()) {}

    void run()
    {
        for (int i = 0; i < 100; ++i)
        {
            total_ += dist(rng);
        }
    }

    int id() const { return id_; }
    int total() const { return total_; }

    void print() const
    {
        std::cout << "thread " << id_ << " - total sum: " << total_ << '\n';
    }
};

int main()
{
    const int NUM_THREADS = 10;

    std::vector<Thread> processors;
    processors.reserve(NUM_THREADS);
    for (int i = 1; i <= NUM_THREADS; ++i)
        processors.emplace_back(i);

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (auto &w : processors)
        threads.emplace_back(&Thread::run, &w);

    for (auto &t : threads)
        t.join();

    std::cout << "results from all threads\n";
    for (const auto &w : processors)
        w.print();

    auto it = std::max_element(
        processors.begin(), processors.end(),
        [](const Thread &a, const Thread &b)
        { return a.total() < b.total(); });

    std::cout << "\nwinner: Thread " << it->id()
              << " with highest score: " << it->total() << '\n';

    return 0;
}

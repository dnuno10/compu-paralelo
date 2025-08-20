#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>
#include <mutex>

// Ejecuta 10 threads, cada uno suma 100 números aleatorios entre 1 y 1000.
// Muestra el resultado de cada thread y el de mayor puntuación.

class Thread
{
    int id_;
    int iterations_;
    // cada objeto va a modificar el mismo vector
    std::vector<int> &totals_;
    uint32_t seed_;

public:
    Thread(int id, int iterations, std::vector<int> &totals,
           uint32_t seed_base = 12345u)
        : id_(id),
          iterations_(iterations),
          totals_(totals),
          seed_(seed_base + static_cast<uint32_t>(id)) {}

    void run()
    {
        std::mt19937 rng(seed_);
        std::uniform_int_distribution<int> dist(1, 1000);

        int local = 0;
        for (int i = 0; i < iterations_; ++i)
            local += dist(rng);

        totals_[id_] = local;
    }
};

int main()
{
    const int NUM_THREADS = 10;
    const int ITER = 100;
    const uint32_t SEED_BASE = 12345u;

    std::vector<int> totals(NUM_THREADS, 0);

    // creamos 10 objetos Thread
    std::vector<Thread> workers;
    workers.reserve(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i)
        workers.emplace_back(i, ITER, totals, SEED_BASE);

    // lanzamos los 10 hilos ejecutando run()
    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i)
        threads.emplace_back(&Thread::run, &workers[i]);

    // para cada hilo esperamos que termine antes de continuar
    for (auto &t : threads)
        t.join();

    std::cout << "results from all threads\n";
    for (int i = 0; i < NUM_THREADS; ++i)
        std::cout << "thread " << (i + 1) << " - total sum: " << totals[i] << '\n';

    auto it = std::max_element(totals.begin(), totals.end());
    // obtenemos el indice del elemento máximo
    int winner_id = static_cast<int>(std::distance(totals.begin(), it));
    std::cout << "\nwinner: Thread " << (winner_id + 1)
              << " with highest score: " << *it << '\n';
    return 0;
}

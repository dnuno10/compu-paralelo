#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <iostream>
#include <print>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>

struct ThreadResult {
    int threadId;
    int totalSum;
    std::vector<int> numbers;
    std::mutex resultMutex;
    
    ThreadResult(int id) : threadId(id), totalSum(0) {}
    
    void addNumber(int number) {
        std::lock_guard<std::mutex> lock(resultMutex);
        numbers.push_back(number);
        totalSum += number;
    }
    
    void printResult() const {
        std::println("Thread {} - Total Sum: {}", threadId, totalSum);
        std::print("Numbers: ");
        for (size_t i = 0; i < numbers.size(); ++i) {
            std::print("{}", numbers[i]);
            if (i < numbers.size() - 1) std::print(", ");
        }
        std::println("");
    }
};

class NumberCalculator {
private:
    int threadId;
    std::shared_ptr<ThreadResult> result;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dis;
    
public:
    NumberCalculator(int id) : threadId(id), gen(rd()), dis(1, 1000) {
        result = std::make_shared<ThreadResult>(id);
    }
    
    std::shared_ptr<ThreadResult> getResult() const {
        return result;
    }
    
    void calculate() {
        std::println("Thread {} started calculating...", threadId);
        
        for (int i = 0; i < 100; ++i) {
            int randomNumber = dis(gen);
            result->addNumber(randomNumber);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        std::println("Thread {} finished calculating. Total: {}", threadId, result->totalSum);
    }
};

class ResultManager {
private:
    std::vector<std::shared_ptr<ThreadResult>> results;
    std::mutex resultsMutex;
    
public:
    void addResult(std::shared_ptr<ThreadResult> result) {
        std::lock_guard<std::mutex> lock(resultsMutex);
        results.push_back(result);
    }
    
    void printAllResults() {
        std::println("\n=== RESULTS FROM ALL THREADS ===");
        for (const auto& result : results) {
            result->printResult();
        }
    }
    
    std::shared_ptr<ThreadResult> findHighestScore() {
        if (results.empty()) return nullptr;
        
        auto maxResult = std::max_element(results.begin(), results.end(),
            [](const std::shared_ptr<ThreadResult>& a, const std::shared_ptr<ThreadResult>& b) {
                return a->totalSum < b->totalSum;
            });
        
        return *maxResult;
    }
    
    void announceWinner() {
        auto winner = findHighestScore();
        if (winner) {
            std::println("\nWINNER: Thread {} with highest score: {}", 
                        winner->threadId, winner->totalSum);
        }
    }
};

int main() {
    const int NUM_THREADS = 10;
    std::vector<std::unique_ptr<NumberCalculator>> calculators;
    std::vector<std::thread> threads;
    ResultManager manager;
    
    std::println("Starting {} threads to calculate random number sums...\n", NUM_THREADS);
    
    for (int i = 1; i <= NUM_THREADS; ++i) {
        calculators.push_back(std::make_unique<NumberCalculator>(i));
    }
    
    for (auto& calc : calculators) {
        threads.emplace_back([&calc, &manager]() {
            calc->calculate();
            manager.addResult(calc->getResult());
        });
    }
        for (auto& t : threads) {
        t.join();
    }
    
    std::println("\nAll threads completed!");
        manager.printAllResults();
    manager.announceWinner();
    
    return 0;
}
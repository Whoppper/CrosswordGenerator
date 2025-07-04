#ifndef ISOLVINGALGORITHMSTRATEGY_HPP
#define ISOLVINGALGORITHMSTRATEGY_HPP

#include <QObject>
#include <chrono>

class CrosswordManager;

class ISolvingAlgorithmStrategy
{
public:
    ISolvingAlgorithmStrategy(int _maxDurationMs) : stopSolving(false), maxDurationMs(_maxDurationMs) {}
    virtual ~ISolvingAlgorithmStrategy() = default;
    virtual bool solve(CrosswordManager& manager) = 0;
    void startTimer() {start = std::chrono::high_resolution_clock::now();}

    int getMaxDurationMs() const {return maxDurationMs;}
    void setMaxDurationMs(int durationMs) {maxDurationMs = durationMs;}
    bool getStopSolving() const {return stopSolving;}
    void setStopSolving() {stopSolving = true;}

    int elapsedTimeMs() const
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        return duration_ms;
    }

protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    bool stopSolving;
    int maxDurationMs;

};

#endif // ISOLVINGALGORITHMSTRATEGY_HPP

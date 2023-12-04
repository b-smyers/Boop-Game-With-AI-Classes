#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>
using namespace std::chrono;

class Timer {
    public:
        Timer(double duration) {
            duration_ms = duration;
        }

        void start() {
            start_time_point = high_resolution_clock::now();
            running = true;
        }

        void stop() {
            end_time_point = high_resolution_clock::now();
            running = false;
        }

        bool times_up() const {
            auto elapsed_ms = elapsedMilliseconds();
            return elapsed_ms >= duration_ms;
        }

        double elapsedMilliseconds() const {
            if (running) {
                auto current_time_point = high_resolution_clock::now();
                return duration<double, std::milli>(current_time_point - start_time_point).count();
            } else {
                return duration<double, std::milli>(end_time_point - start_time_point).count();
            }
        }

    private:
        double duration_ms;
        time_point<high_resolution_clock> start_time_point;
        time_point<high_resolution_clock> end_time_point;
        bool running = false;
};

#endif
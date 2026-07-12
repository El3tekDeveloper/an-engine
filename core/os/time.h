#pragma once
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

class TimeSystem {
public:
    TimeSystem() {
        _real_time = std::chrono::duration<double>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }
    ~TimeSystem() = default;
    
    static TimeSystem& get_singleton() {
        static TimeSystem instance;
        return instance;
    }

    double delta_time() { return _delta_time; }
    double unscaled_delta_time() { return _unscaled_delta_time; }
    double real_time() { return _real_time; }
    double time() { return _time; }

    uint32_t frame_count() { return _frame_count; }
    float time_scale = 1.0f;

    void update() {
        auto now = std::chrono::steady_clock::now();

        double current =
            std::chrono::duration<double>(
                    now.time_since_epoch()
                    ).count();

        _unscaled_delta_time = current - _real_time;
        _delta_time = _unscaled_delta_time * time_scale;

        _real_time = current;
        _time += _delta_time;

        _frame_count++;
    }
    
    std::string get_local_time_format() {
        auto t = std::time(nullptr); 
        auto tm = std::localtime(&t); 
        std::ostringstream os; 
        os << std::put_time(tm, "%H:%M:%S %d=%m=%Y"); 
        return os.str();
    }

private:
    double _delta_time = 0.0f;
    double _unscaled_delta_time = 0.0f;
    double _real_time = 0.0f;
    double _time = 0.0f;
    
    uint32_t _frame_count = 0;
};

static TimeSystem& Time = TimeSystem::get_singleton();

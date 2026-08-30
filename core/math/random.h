#pragma once
#include <cstdlib>

namespace Random {

inline float rangef(float min_v, float max_v) {
    return min_v + (max_v - min_v) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

inline float range(float min_v, float max_v) {
    return min_v + (max_v - min_v) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

inline int range(int min_v, int max_v) {
    return min_v + (max_v - min_v) * (static_cast<int>(rand()) / static_cast<int>(RAND_MAX));
}

}

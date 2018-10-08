#include "math/Random.h"
#include <random>

float Math::GetRandomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.f, 1.f);
    return dis(gen);
}

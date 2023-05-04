
#include "RandomGenerator.h"
#include <random>


namespace uncanny
{


u32 FRandomGenerator::GetSeed()
{
  std::random_device randomDevice{};
  std::uniform_int_distribution<u32> distribution{};
  return distribution(randomDevice);
}


}

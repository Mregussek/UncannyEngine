
#include "IdentifierRegistry.h"
#include <random>
#include <cstdint>
#include <ctime>


namespace uncanny
{


u64 FIdentifierRegistry::GenerateID() {
  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<> dis(0, 999);
  i64 randomNumber = dis(gen);

  std::time_t now = std::time(nullptr);
  u64 timestamp = now;

  u64 identifier = timestamp << 10 | randomNumber;
  return identifier;
}


}

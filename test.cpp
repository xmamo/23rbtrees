#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "map.hpp"

namespace {
#ifndef NDEBUG
  void check(std::size_t count, std::default_random_engine& engine) {
    Map<int, int> map;

    {
      std::vector<int> keys(count);
      std::iota(keys.begin(), keys.end(), 0);
      std::shuffle(keys.begin(), keys.end(), engine);

      for (int key : keys) {
        map.insert(key, -key);
        map.check();
        int* value = map.lookup(key);
        assert(value != nullptr && *value == -key);
      }

      std::shuffle(keys.begin(), keys.end(), engine);

      for (int key : keys) {
        int* value = map.lookup(key);
        assert(value != nullptr && *value == -key);
      }

      {
        Map<int, int> map_copy(map);
        map_copy.check();

        for (int key : keys) {
          int* value = map_copy.lookup(key);
          assert(value != nullptr && *value == -key);
        }
      }

      std::shuffle(keys.begin(), keys.end(), engine);

      for (int key : keys) {
        map.remove(key);
        map.check();
        assert(map.lookup(key) == nullptr);
      }

      assert(map.count() == 0);
    }

    {
      enum Operation {
        INSERT,
        LOOKUP,
        REMOVE,
      };

      std::vector<std::pair<Operation, int>> operation_key_pairs;

      for (std::size_t i = 0; i < count; ++i) {
        operation_key_pairs.emplace_back(INSERT, i);
        operation_key_pairs.emplace_back(LOOKUP, i);
        operation_key_pairs.emplace_back(REMOVE, i);
      }

      std::shuffle(operation_key_pairs.begin(), operation_key_pairs.end(), engine);

      for (auto [operation, key] : operation_key_pairs) {
        switch (operation) {
          case LOOKUP: {
            int* value = map.lookup(key);
            assert(value == nullptr || *value == -key);
            break;
          }

          case INSERT: {
            map.insert(key, -key);
            map.check();
            int* value = map.lookup(key);
            assert(value != nullptr && *value == -key);
            break;
          }

          case REMOVE: {
            map.remove(key);
            assert(map.lookup(key) == nullptr);
            break;
          }
        }
      }
    }
  }
#else
#define check(count, engine)
#endif

  volatile int value;
  int* volatile value_p;

  template <typename Rep, typename Period>
  std::ostream& operator<<(std::ostream& os, std::chrono::duration<Rep, Period> duration) {
    return os << std::right << std::setw(std::numeric_limits<Rep>::digits10) << duration.count();
  }

  void bench(std::size_t count, std::default_random_engine& engine) {
    std::map<int, int> std_map;
    Map<int, int> map;

    {
      std::vector<int> keys(count);
      std::iota(keys.begin(), keys.end(), 0);
      std::shuffle(keys.begin(), keys.end(), engine);

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          std_map[key] = -key;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "       std::map insertions: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          map.insert(key, -key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "            Map insertions: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();
        std::map<int, int> std_map_copy(std_map);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "             std::map copy: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        Map<int, int> map_copy(map);
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "                  Map copy: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        std_map_copy.clear();
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "            std::map clear: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        map_copy.clear();
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "                 Map clear: " << t1 - t0 << std::endl;
      }

      std::shuffle(keys.begin(), keys.end(), engine);

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          value = std_map[key];
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "          std::map lookups: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          value_p = map.lookup(key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "               Map lookups: " << t1 - t0 << std::endl;
      }

      std::shuffle(keys.begin(), keys.end(), engine);

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          std_map.erase(key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "         std::map removals: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          map.remove(key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "              Map removals: " << t1 - t0 << std::endl;
      }
    }

    {
      enum Operation {
        INSERT,
        LOOKUP,
        REMOVE,
      };

      std::vector<std::pair<Operation, int>> operation_key_pairs;

      for (std::size_t i = 0; i < count; ++i) {
        operation_key_pairs.emplace_back(INSERT, i);
        operation_key_pairs.emplace_back(LOOKUP, i);
        operation_key_pairs.emplace_back(REMOVE, i);
      }

      std::shuffle(operation_key_pairs.begin(), operation_key_pairs.end(), engine);

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (auto [operation, key] : operation_key_pairs) {
          switch (operation) {
            case LOOKUP:
              value = std_map[key];
              break;

            case INSERT:
              std_map[key] = -key;
              break;

            case REMOVE:
              std_map.erase(key);
              break;
          }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "std::map random operations: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (auto [operation, key] : operation_key_pairs) {
          switch (operation) {
            case LOOKUP:
              value_p = map.lookup(key);
              break;

            case INSERT:
              map.insert(key, -key);
              break;

            case REMOVE:
              map.remove(key);
              break;
          }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "     Map random operations: " << t1 - t0 << std::endl;
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [shift]" << std::endl;
    return EXIT_FAILURE;
  }

  std::size_t count = static_cast<std::size_t>(1) << 10;

  if (argc > 1) {
    int shift;

    try {
      shift = std::stoi(std::string(argv[1]));
    } catch (const std::invalid_argument&) {
      std::cerr << "Usage: " << argv[0] << " [shift]" << std::endl;
      return EXIT_FAILURE;
    } catch (const std::out_of_range&) {
      std::cerr << "<shift> out of range" << std::endl;
      return EXIT_FAILURE;
    }

    count = static_cast<std::size_t>(1) << shift;
  }

  std::default_random_engine engine((std::random_device()()));
  check(count, engine);
  bench(count, engine);
  return EXIT_SUCCESS;
}

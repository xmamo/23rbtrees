#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

extern "C" {
#include "comparator.h"
#include "layout.h"
#include "map.h"
}

namespace cpp {
#include "map.hpp"
}

namespace {
  volatile int value;
  int* volatile value_p;

#ifndef NDEBUG
  void check(std::size_t count, std::default_random_engine& engine) {
    {
      cpp::Map<int, int> cpp_map;

      {
        std::vector<int> keys(count);
        std::iota(keys.begin(), keys.end(), 0);
        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          cpp_map.insert(key, -key);
          cpp_map.check();
          value_p = cpp_map.lookup(key);
          assert(value_p != nullptr && *value_p == -key);
        }

        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          value_p = cpp_map.lookup(key);
          assert(value_p != nullptr && *value_p == -key);
        }

        {
          cpp::Map<int, int> cpp_map_copy(cpp_map);
          cpp_map_copy.check();

          for (int key : keys) {
            value_p = cpp_map_copy.lookup(key);
            assert(value_p != nullptr && *value_p == -key);
          }
        }

        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          cpp_map.remove(key);
          cpp_map.check();
          assert(cpp_map.lookup(key) == nullptr);
        }

        assert(cpp_map.count() == 0);
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
            case LOOKUP:
              value_p = cpp_map.lookup(key);
              assert(value_p == nullptr || *value_p == -key);
              break;

            case INSERT:
              cpp_map.insert(key, -key);
              cpp_map.check();
              value_p = cpp_map.lookup(key);
              assert(value_p != nullptr && *value_p == -key);
              break;

            case REMOVE:
              cpp_map.remove(key);
              cpp_map.check();
              assert(cpp_map.lookup(key) == nullptr);
              break;
          }
        }
      }
    }

    {
      Map* c_map = map_new(
        Layout{sizeof(int), alignof(int)},
        Layout{sizeof(int), alignof(int)},
        int_comparator
      );

      {
        std::vector<int> keys(count);
        std::iota(keys.begin(), keys.end(), 0);
        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          value = -key;
          map_insert(c_map, &key, const_cast<int*>(&value));
          map_check(c_map);
          value_p = static_cast<int*>(map_lookup(c_map, &key));
          assert(value_p != NULL && *value_p == value);
        }

        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          value_p = static_cast<int*>(map_lookup(c_map, &key));
          assert(value_p != NULL && *value_p == -key);
        }

        {
          struct Map* c_map_copy = map_copy(c_map);
          map_check(c_map_copy);

          for (int key : keys) {
            value_p = static_cast<int*>(map_lookup(c_map_copy, &key));
            assert(value_p != NULL && *value_p == -key);
          }

          map_destroy(c_map_copy);
        }

        std::shuffle(keys.begin(), keys.end(), engine);

        for (int key : keys) {
          map_remove(c_map, &key);
          map_check(c_map);
          assert(map_lookup(c_map, &key) == NULL);
        }

        assert(map_count(c_map) == 0);
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
            case LOOKUP:
              value_p = static_cast<int*>(map_lookup(c_map, &key));
              assert(value_p == NULL || *value_p == -key);
              break;

            case INSERT:
              value = -key;
              map_insert(c_map, &key, const_cast<int*>(&value));
              map_check(c_map);
              value_p = static_cast<int*>(map_lookup(c_map, &key));
              assert(value_p != NULL && *value_p == value);
              break;

            case REMOVE:
              map_remove(c_map, &key);
              map_check(c_map);
              assert(map_lookup(c_map, &key) == NULL);
              break;
          }
        }
      }

      map_destroy(c_map);
    }
  }
#else
#define check(count, engine)
#endif

  template <typename Rep, typename Period>
  std::ostream& operator<<(std::ostream& os, std::chrono::duration<Rep, Period> duration) {
    return os << std::right << std::setw(std::numeric_limits<Rep>::digits10) << duration.count();
  }

  void bench(std::size_t count, std::default_random_engine& engine) {
    std::map<int, int> std_map;
    cpp::Map<int, int> cpp_map;

    Map* c_map = map_new(
      Layout{sizeof(int), alignof(int)},
      Layout{sizeof(int), alignof(int)},
      int_comparator
    );

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
          cpp_map.insert(key, -key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "        C++ Map insertions: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          value = -key;
          map_insert(c_map, &key, const_cast<int*>(&value));
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "          C Map insertions: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();
        std::map<int, int> std_map_copy(std_map);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "             std::map copy: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        cpp::Map<int, int> cpp_map_copy(cpp_map);
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "              C++ Map copy: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        Map* c_map_copy = map_copy(c_map);
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "                C Map copy: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        std_map_copy.clear();
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "            std::map clear: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        cpp_map_copy.clear();
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "             C++ Map clear: " << t1 - t0 << std::endl;

        t0 = std::chrono::high_resolution_clock::now();
        map_clear(c_map_copy);
        t1 = std::chrono::high_resolution_clock::now();
        std::cout << "               C Map clear: " << t1 - t0 << std::endl;

        map_destroy(c_map_copy);
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
          value_p = cpp_map.lookup(key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "           C++ Map lookups: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          value_p = static_cast<int*>(map_lookup(c_map, &key));
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "             C Map lookups: " << t1 - t0 << std::endl;
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
          cpp_map.remove(key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "          C++ Map removals: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (int key : keys) {
          map_remove(c_map, &key);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "            C Map removals: " << t1 - t0 << std::endl;
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
              value_p = cpp_map.lookup(key);
              break;

            case INSERT:
              cpp_map.insert(key, -key);
              break;

            case REMOVE:
              cpp_map.remove(key);
              break;
          }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << " C++ Map random operations: " << t1 - t0 << std::endl;
      }

      {
        auto t0 = std::chrono::high_resolution_clock::now();

        for (auto [operation, key] : operation_key_pairs) {
          switch (operation) {
            case LOOKUP:
              value_p = static_cast<int*>(map_lookup(c_map, &key));
              break;

            case INSERT: {
              value = -key;
              map_insert(c_map, &key, const_cast<int*>(&value));
              break;
            }

            case REMOVE:
              map_remove(c_map, &key);
              break;
          }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "   C Map random operations: " << t1 - t0 << std::endl;
      }
    }

    map_destroy(c_map);
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

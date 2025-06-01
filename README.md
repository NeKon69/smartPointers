# smartPointers: A Custom C++ Smart Pointer Implementation

## Crafting Modern Memory Management from Scratch

This project offers a self-contained implementation of modern C++ smart pointers: `unique_ptr`, `shared_ptr`, and `weak_ptr`. Developed primarily as an educational exercise and a deep dive into the intricacies of memory management in C++, it aims to replicate the core functionalities and behavior of their `std` counterparts.

Beyond simply reimplementing the standard library features, `smartPointers` provides a robust test suite, including both comprehensive unit tests for correctness and detailed performance benchmarks comparing our custom `raw::` pointers against `std::` pointers. This allows for a direct evaluation of their efficiency and overhead.

Authored by [NeKon69](https://github.com/NeKon69/smartPointers).

## Table of Contents

*   [Key Features](#key-features)
*   [Implemented Smart Pointers](#implemented-smart-pointers)
    *   [`raw::unique_ptr`](#rawunique_ptr)
    *   [`raw::shared_ptr`](#rawshared_ptr)
    *   [`raw::weak_ptr`](#rawweak_ptr)
*   [Technical Details](#technical-details)
*   [Getting Started](#getting-started)
    *   [Prerequisites](#prerequisites)
    *   [Building from Source](#building-from-source)
    *   [Running Tests and Benchmarks](#running-tests-and-benchmarks)
*   [Performance Benchmarks](#performance-benchmarks)
    *   [`raw::unique_ptr` vs `std::unique_ptr`](#rawunique_ptr-vs-stdunique_ptr)
    *   [`raw::shared_ptr` vs `std::shared_ptr`](#rawshared_ptr-vs-stdshared_ptr)
    *   [`raw::weak_ptr` vs `std::weak_ptr`](#rawweak_ptr-vs-stdweak_ptr)
    *   [Benchmark Summary](#benchmark-summary)
*   [Project Structure](#project-structure)
*   [License](#license)
*   [Contact](#contact)

## Key Features

*   **Complete Smart Pointer Trio:** Full implementations of `raw::unique_ptr`, `raw::shared_ptr`, and `raw::weak_ptr`.
*   **Single Object and Array Support:** All pointer types correctly handle both single objects (`T`) and dynamic arrays (`T[]`).
*   **Safe Factory Functions:** Includes `raw::make_unique` and `raw::make_shared` for safe, exception-aware, and efficient (for `shared_ptr`) object creation.
*   **Thread-Safe Reference Counting:** `raw::shared_ptr` and `raw::weak_ptr` utilize `std::atomic` for thread-safe manipulation of use and weak counts.
*   **Custom Control Block Optimization:** `raw::make_shared` optimizes memory allocation by allocating the object and its control block in a single contiguous memory region.
*   **Comprehensive Unit Tests:** Extensive test cases ensure the correctness and adherence to expected behavior for all smart pointer operations.
*   **Detailed Performance Benchmarks:** In-depth comparisons against `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr` to quantify performance characteristics.
*   **Memory Leak Detection:** Unit tests incorporate a counter for active `TestObject` instances, verifying proper memory deallocation and absence of leaks.

## Implemented Smart Pointers

### `raw::unique_ptr`

Provides exclusive ownership of a dynamically allocated object or array. It implements move semantics, preventing copying, and ensures automatic deallocation when it goes out of scope.

### `raw::shared_ptr`

Enables shared ownership of an object or array. It maintains a reference count (via a control block) to track how many `shared_ptr` instances point to the same resource. The resource is deallocated only when the last `shared_ptr` owning it is destroyed.

### `raw::weak_ptr`

A non-owning "observer" smart pointer that can point to an object managed by a `raw::shared_ptr` without affecting its reference count. It's crucial for breaking circular references that would otherwise lead to memory leaks with `shared_ptr`. The `lock()` method can be used to safely obtain a `shared_ptr` if the observed object still exists.

## Technical Details

*   **Language Standard:** C++23
*   **Build System:** CMake
*   **Concurrency Primitives:** `std::atomic` for thread-safe reference counting.
*   **Memory Management:** Leverages `new`/`delete` for `unique_ptr` and custom allocation/deallocation routines (e.g., `std::aligned_alloc`, `std::free`, placement new) for `shared_ptr` and `weak_ptr` control blocks to optimize memory layout and performance.
*   **Custom Deleters:** Internal helper functions manage the correct destruction of single objects or arrays.

## Getting Started

### Prerequisites

To build and run this project, you will need:

*   A C++23 compatible compiler (e.g., GCC 13+, Clang 16+, MSVC 19.36+)
*   CMake version 3.20 or higher
*   Git

### Building from Source

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/NeKon69/smartPointers.git
    cd smartPointers
    ```
2.  **Configure the build:**
    ```bash
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    ```
    This command creates a `build` directory and prepares the project files inside it for a Release build.
3.  **Compile the code:**
    ```bash
    cmake --build build --config Release
    ```
    This will compile all source files and generate the executable.

### Running Tests and Benchmarks

The compiled executable automatically runs all unit tests and performance benchmarks.

*   **On Linux/macOS:**
    ```bash
    ./build/smartPointers
    ```
*   **On Windows (if using Visual Studio build):**
    ```bash
    .\build\Release\smartPointers.exe
    ```

The output will show detailed results for each unit test and then present performance comparisons in a structured table.

## Performance Benchmarks

The project includes a comprehensive set of benchmarks comparing `raw::` smart pointers against their `std::` counterparts. Each scenario is run for `NUM_TRIALS` (200) iterations with `OPS_PER_TRIAL` (1,000,000) operations. Results are presented in microseconds (us), showing minimum, maximum, and average durations, along with the percentage difference of `raw` vs `std` average time.

All benchmarks were performed on **Arch Linux** using **GCC**.

### `raw::unique_ptr` vs `std::unique_ptr`

```
--- Performance Comparison Test: raw::unique_ptr vs std::unique_ptr ---

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Scenario                        |                     STD (us) |                     RAW (us) |                         Comparison | Active Objects Start/End
                                |     Min      Max         Avg |     Min      Max         Avg |                 RAW vs STD Avg (%) |     Pre-test   Post-test
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Make Single Object              |     198      228         205 |     198      219         204 |                          -0.49 |            0           0
Make Array (size 1-10)          |   10497    11908       10973 |   10373    14420       11382 |                          +3.73 |            0           0
Reset Single Object             |    4235     5046        4747 |    4392     5023        4745 |                          -0.04 |            0           0
Reset Array                     |   11749    12456       12078 |   11902    12591       12231 |                          +1.27 |            0           0
Move Construct Single           |    1026     1245        1067 |    1028     1174        1077 |                          +0.94 |            0           0
Move Assign Single              |     600      634         609 |     417      596         434 |                         -28.74 |            0           0
Move Assign Array               |     601      620         605 |     418      588         437 |                         -27.77 |            0           0
Access Single Object            |     726      826         761 |     729     1138         791 |                          +3.94 |            0           0
Access Array Element            |    1341     1548        1416 |    1339     1590        1408 |                          -0.56 |            0           0
Combined Stress Test            |    2035     2083        2052 |    2056     2183        2088 |                          +1.75 |            0           0
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Performance comparison finished.
```

### `raw::shared_ptr` vs `std::shared_ptr`

```
--- Performance Comparison Test: raw::shared_ptr vs std::shared_ptr ---

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Scenario                            |                     STD (us) |                     RAW (us) |                         Comparison | Active Objects Start/End
                                    |     Min      Max         Avg |     Min      Max         Avg |                 RAW vs STD Avg (%) |     Pre-test   Post-test
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Make Single Object (make_shared)    |    7284     8310        7647 |    6341     7731        6687 |                             -12.55 |            0           0
Make Array (make_shared, size 1-10) |   12770    14012       12952 |   12197    13943       12611 |                              -2.63 |            0           0
Reset Single Object                 |    5761     7314        6278 |    5303     6043        5612 |                             -10.61 |            0           0
Reset Array                         |    5904     6831        6183 |    5527     6925        5801 |                              -6.18 |            0           0
Move Construct Single               |    7030     8419        7338 |    6619     7307        6828 |                              -6.95 |            0           0
Move Assign Single                  |   14380    15049       14563 |   13018    20432       13943 |                              -4.26 |            0           0
Move Assign Array                   |   23243    27527       24796 |   22911    28927       23539 |                              -5.07 |            0           0
Access Single Object                |     739     1015         880 |    1668     2025        1794 |                            +103.86 |            0           0
Access Array Element                |    3923     6323        4401 |    4267     5437        4508 |                              +2.43 |            0           0
Copy Construct Single               |   12566    12757       12605 |    6472     6724        6645 |                             -47.28 |            0           0
Copy Assign Single                  |    1698     1936        1787 |    3201     3748        3308 |                             +85.11 |            0           0
Copy Assign Array                   |    3368     4526        3509 |    3411     3895        3565 |                              +1.60 |            0           0
Use Count                           |     297      372         347 |     198      378         233 |                             -32.85 |            0           0
Unique Check                        |     396      503         409 |       0        0           0 |                            -100.00 |            0           0
Combined Stress Test (Shared)       |   21056    24045       21677 |   20797    22854       21193 |                              -2.23 |            0           0
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Performance comparison finished.
```

### `raw::weak_ptr` vs `std::weak_ptr`

```
--- Performance Comparison Test: raw::weak_ptr vs std::weak_ptr ---

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Scenario                            |                     STD (us) |                     RAW (us) |                         Comparison | Active Objects Start/End
                                    |     Min      Max         Avg |     Min      Max         Avg |                 RAW vs STD Avg (%) |     Pre-test   Post-test
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Weak from Shared Creation (Single)  |   10742    10788       10752 |    1848     2330        1987 |                             -81.52 |            0           0
Weak from Shared Creation (Array)   |   21815    27238       22418 |   12280    14129       12571 |                             -43.92 |            0           0
Weak Copy Construct                 |   18314    20496       18847 |    6637     9289        7169 |                             -61.96 |            0           0
Weak Move Construct                 |   18349    18474       18393 |    6273     7253        6564 |                             -64.31 |            0           0
Weak Copy Assign                    |   20522    21337       20749 |   13511    14470       13715 |                             -33.90 |            0           0
Weak Move Assign                    |   20672    21704       20923 |   13001    13409       13076 |                             -37.50 |            0           0
Weak Shared Assign (Single)         |   18314    19253       18603 |    6417     6754        6606 |                             -64.49 |            0           0
Weak Shared Assign (Array)          |   21911    23556       22308 |   12383    12661       12444 |                             -44.22 |            0           0
Weak Reset                          |    8463     9292        8642 |    6307     7049        6553 |                             -24.17 |            0           0
Weak Swap                           |   23132    24202       23405 |   12733    12869       12782 |                             -45.39 |            0           0
Weak Use Count                      |     249      372         293 |     198      227         201 |                             -31.40 |            0           0
Weak Expired                        |    4608     4984        4708 |    4679     4865        4787 |                              +1.68 |            0           0
Weak Lock (Single)                  |    8609     8774        8688 |    4605     4724        4667 |                             -46.28 |            0           0
Weak Lock (Array)                   |    9046     9953        9188 |    7685     7841        7745 |                             -15.71 |            0           0
Combined Stress Test (Weak)         |   20790    21443       20913 |   20449    21324       20687 |                              -1.08 |            0           0
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
```

### Benchmark Summary

These benchmarks highlight several key observations:

*   **`raw::unique_ptr`**: Generally performs very close to `std::unique_ptr`. Our custom implementation shows a noticeable performance advantage in move assignment scenarios (both single objects and arrays), while other operations are comparable or slightly slower. This suggests a lean design with minimal overhead.
*   **`raw::shared_ptr`**: `make_shared`, `reset`, and `copy construct` operations for `raw::shared_ptr` show performance advantages compared to `std::shared_ptr`. `use_count()` and `unique()` checks are also reported as significantly faster for `raw`. However, operations like `copy assign` and `access single object` are notably slower for `raw::shared_ptr`. This indicates a mixed performance profile, where efficiency varies depending on the specific operation.
*   **`raw::weak_ptr`**: In the vast majority of tested operations, including creation from shared pointers, copy/move construction/assignment, reset, swap, use count, and lock, `raw::weak_ptr` demonstrates significantly lower execution times (i.e., faster performance) than `std::weak_ptr`. The `expired()` check is the only operation where `raw::weak_ptr` is slightly slower.

Overall, the benchmarks suggest that `raw::unique_ptr` is highly competitive and faster in specific scenarios. `raw::weak_ptr` appears to outperform `std::weak_ptr` in almost all measured operations according to this data. `raw::shared_ptr` shows advantages in some key operations (like creation and reset) but is slower in others (like copy assignment and single object access), resulting in a mixed performance profile compared to its `std::` counterpart.

## Project Structure

The project is organized into the following main directories:

*   `include/raw/`: Contains all public header files defining the `raw::` smart pointer classes (`fwd.h`, `hub.h`, `helper.h`, `smart_ptr_base.h`, `unique_ptr.h`, `shared_ptr.h`, `weak_ptr.h`). `raw_memory.h` serves as a convenient single-include header.
*   `tests/`: Houses the unit tests (`unit_unique.h`, `unit_shared.h`, `unit_weak.h`) and performance benchmarks (`benchmark_unique.h`, `benchmark_shared.h`, `benchmark_weak.h`, `common_test_utils.h`, `run_all_tests.h`) for the smart pointer implementations.
*   `CMakeLists.txt`: The main CMake build configuration file for the project.
*   `main.cpp`: The entry point for running all tests and benchmarks.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE.txt) file for details.

## Contact

If you have any questions, suggestions, or encounter issues, feel free to reach out:

*   **GitHub Issues:** [https://github.com/NeKon69/smartPointers/issues](https://github.com/NeKon69/smartPointers/issues)
*   **Email:** nobodqwe@gmail.com

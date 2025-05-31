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
Make Single Object (make_shared)    |    7152     8286        7393 |    6756     7310        7018 |                          -5.07 |            0           0
Make Array (make_shared, size 1-10) |   12601    14716       12958 |   12129    12839       12308 |                          -5.02 |            0           0
Reset Single Object                 |    5301     6402        5850 |    5307     7282        5717 |                          -2.27 |            0           0
Reset Array                         |    5880     6834        6161 |    5368     6148        5698 |                          -7.52 |            0           0
Move Construct Single               |    1408     1577        1465 |    1918     2056        1961 |                         +33.86 |            0           0
Move Assign Single                  |     669      855         708 |     740      775         753 |                          +6.36 |            0           0
Move Assign Array                   |     669      809         712 |     750      826         781 |                          +9.69 |            0           0
Access Single Object                |     198      206         199 |     198      216         200 |                          +0.50 |            0           0
Access Array Element                |    2176     2395        2249 |    2730     3084        2823 |                         +25.52 |            0           0
Copy Construct Single               |    2510     2759        2620 |    4891     5272        5009 |                         +91.18 |            0           0
Copy Assign Single                  |    5715     6490        5965 |    7270     8683        7752 |                         +29.96 |            0           0
Copy Assign Array                   |    7281     8824        7609 |    7956     8725        8202 |                          +7.79 |            0           0
Use Count                           |     804     1182         892 |    1691     1944        1791 |                        +100.78 |            0           0
Unique Check                        |     863     1021         917 |    1762     1956        1835 |                        +100.11 |            0           0
Combined Stress Test (Shared)       |   22934    23934       23299 |   22975    23321       23143 |                          -0.67 |            0           0
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
Weak from Shared Creation (Single)  |    1262     1452        1303 |    4338     4498        4390 |                        +236.91 |            0           0
Weak from Shared Creation (Array)   |    2714     3037        2843 |    4559     5084        4652 |                         +63.63 |            0           0
Weak Copy Construct                 |    2443     2728        2557 |    4371     5053        4553 |                         +78.06 |            0           0
Weak Move Construct                 |    1355     1556        1427 |    1486     1637        1555 |                          +8.97 |            0           0
Weak Copy Assign                    |    1652     1883        1720 |    3545     3841        3631 |                        +111.10 |            0           0
Weak Move Assign                    |     542     1085         687 |     597     1094         674 |                          -1.89 |            0           0
Weak Shared Assign (Single)         |    1749     2077        1849 |    3290     3751        3474 |                         +87.89 |            0           0
Weak Shared Assign (Array)          |    3279     3718        3463 |    3698     4037        3834 |                         +10.71 |            0           0
Weak Reset                          |    1280     1417        1326 |    3082     3377        3170 |                        +139.06 |            0           0
Weak Swap                           |     774      990         866 |     796     1030         887 |                          +2.42 |            0           0
Weak Use Count                      |     909     1131         949 |    1841     2049        1921 |                        +102.42 |            0           0
Weak Expired                        |     869     1135         965 |    1779     2090        1864 |                         +93.16 |            0           0
Weak Lock (Single)                  |    3803     3992        3871 |    3531     4220        3628 |                          -6.28 |            0           0
Weak Lock (Array)                   |    4235     4393        4280 |    4262     4875        4375 |                          +2.22 |            0           0
Combined Stress Test (Weak)         |   16658    16926       16764 |   17081    17529       17228 |                          +2.77 |            0           0
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
```

### Benchmark Summary

These benchmarks highlight several key observations:

*   **`raw::unique_ptr`**: Generally performs very close to `std::unique_ptr`, and in some move assignment scenarios, our custom implementation shows a noticeable performance advantage. This suggests a lean design with minimal overhead.
*   **`raw::shared_ptr`**: While `make_shared` and `reset` operations for `raw::shared_ptr` are competitive or slightly faster, copy construction/assignment, `use_count()`, and `unique()` checks appear to incur more overhead compared to `std::shared_ptr`. This is likely due to the highly optimized, potentially compiler-specific, atomic operations and control block management within `std::shared_ptr` that are hard to match without compiler intrinsics.
*   **`raw::weak_ptr`**: In most operations, `raw::weak_ptr` demonstrates higher execution times than `std::weak_ptr`. This also points to the overhead associated with atomic operations on the control block, especially during construction from `shared_ptr` and copy/assignment operations. The `lock()` method, however, shows comparable or even slightly better performance for single objects.

Overall, while `raw::unique_ptr` proves to be highly efficient, the `raw::shared_ptr` and `raw::weak_ptr` implementations, while fully functional and thread-safe, might not outperform `std::` counterparts in all scenarios due to the extensive low-level optimizations present in standard library implementations.

## Project Structure

The project is organized into the following main directories:

*   `include/raw/`: Contains all public header files defining the `raw::` smart pointer classes (`fwd.h`, `hub.h`, `helper.h`, `smart_ptr_base.h`, `unique_ptr.h`, `shared_ptr.h`, `weak_ptr.h`). `raw_memory.h` serves as a convenient single-include header.
*   `tests/`: Houses the unit tests (`unit_unique.h`, `unit_shared.h`, `unit_weak.h`) and performance benchmarks (`benchmark_unique.h`, `benchmark_shared.h`, `benchmark_weak.h`, `common_test_utils.h`, `run_all_tests.h`) for the smart pointer implementations.
*   `CMakeLists.txt`: The main CMake build configuration file for the project.
*   `main.cpp`: The entry point for running all tests and benchmarks.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

If you have any questions, suggestions, or encounter issues, feel free to reach out:

*   **GitHub Issues:** [https://github.com/NeKon69/smartPointers/issues](https://github.com/NeKon69/smartPointers/issues)
*   **Email:** nobodqwe@gmail.com

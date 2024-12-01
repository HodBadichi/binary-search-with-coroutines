# README: Binary Search with Coroutines

## Overview

This project explores the use of C++ coroutines to implement a parallel binary search algorithm. The goal is to achieve higher memory-level parallelism by interleaving the execution of multiple binary search instances.

## Building and Running

**Prerequisites:**
* A C++20 compatible compiler (e.g., Clang 16 or GCC 11)
* CMake (optional, for a more flexible build system)

## Running
`./gcc-binary-search --group_size <group_size> --array_size <array_size> --targets_size <targets_size>`

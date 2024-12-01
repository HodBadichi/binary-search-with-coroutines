#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <coroutine>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

using namespace std;
struct Arguments {
  int group_size;
  int array_size;
  int targets_size;
};

struct Search_ctx {
  int *targets;
  int *arr;
  int arr_size;
  int targets_arr_size;
  int group_size;
};

Search_ctx *create_ctx(const int arraySize,
                       const int targetsArraySize, const int group_size);

void run_interleaved_coroutine(Search_ctx *ctx, int *exp_targets,
                               std::string search_type);

void run_serial(Search_ctx *ctx, int *targets_reg, std::string name);

void print_results(Search_ctx *ctx, std::string name,
                   chrono::duration<double> duration);
void generate_unique_sorted_random_array(int arraySize, int *arr);

Arguments get_arguments(int argc, char **argv);

void validate_target_arrays(int *targets_reg, int *targets_interleaved,
                            int *targets_branchless,
                            int *targets_branchless_interleaved, int *targets,
                            int *arr, const int targetsArraySize);
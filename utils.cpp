#include "common.hpp"

void generate_unique_sorted_random_array(int arraySize, int *arr) {
  for (int i = 0; i < arraySize; i++) {
    arr[i] = i * 2 + 1;
  }
}

Arguments get_arguments(int argc, char **argv) {
  Arguments args;
  bool group_size_set = false, array_size_set = false, targets_size_set = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--group_size" && i + 1 < argc) {
      args.group_size = std::stoi(argv[++i]);
      group_size_set = true;
    } else if (arg == "--array_size" && i + 1 < argc) {
      args.array_size = std::stoi(argv[++i]);
      array_size_set = true;
    } else if (arg == "--targets_size" && i + 1 < argc) {
      args.targets_size = std::stoi(argv[++i]);
      targets_size_set = true;
    } else {
      throw std::invalid_argument("Invalid argument: " + arg);
    }
  }

  if (!group_size_set || !array_size_set || !targets_size_set) {
    throw std::invalid_argument(
        "Missing required arguments. Usage: --group_size <int> --array_size "
        "<int> --targets_size <int>");
  }

  if (args.array_size <= args.targets_size) {
    throw std::invalid_argument("array_size must be larger than targets_size.");
  }
  return args;
}

void print_results(Search_ctx *ctx, std::string name,
                   chrono::duration<double> duration) {
  cout << "Throughput for " << ctx->targets_arr_size / 1e6 << "[M] ops " << name
       << ":" << (ctx->targets_arr_size / 1e6) / duration.count() << " [Mops/s]"
       << endl;
}

Search_ctx *create_ctx(const int arraySize, const int targetsArraySize,
                       const int group_size) {
  int targetIndex;
  Search_ctx *ctx = (Search_ctx *)malloc(sizeof(Search_ctx));
  ctx->arr = (int *)malloc(arraySize * sizeof(int));
  ctx->targets = (int *)malloc(targetsArraySize * sizeof(int));

  if (ctx->arr == nullptr || ctx->targets == nullptr || ctx == nullptr) {
    std::cerr << "Failed allocation" << std::endl;
    return nullptr;
  }

  generate_unique_sorted_random_array(arraySize, ctx->arr);

  // Choose a random target value from the array
  for (int i = 0; i < targetsArraySize; i++) {
    targetIndex = rand() % arraySize;
    ctx->targets[i] = ctx->arr[targetIndex];
  }
  ctx->arr_size = arraySize;
  ctx->targets_arr_size = targetsArraySize;
  ctx->group_size = group_size;
  return ctx;
}

void assert_equal(int expected, int actual, const char *message) {
  if (expected != actual) {
    std::cerr << "Assertion failed: " << message << std::endl;
    std::cerr << "Expected: " << expected << ", Actual: " << actual
              << std::endl;
    // Optionally, log the failure or throw an exception
    std::exit(1); // Or use a more graceful error handling mechanism
  }
}

void validate_target_arrays(int *targets_reg, int *targets_interleaved,
                            int *targets_branchless,
                            int *targets_branchless_interleaved, int *targets,
                            int *arr, const int targetsArraySize) {
  std::cout << "Validating results..." << std::endl;

  for (int i = 0; i < targetsArraySize; ++i) {
    assert_equal(targets_reg[i], targets_interleaved[i],
                 "Interleaved targets mismatch");
    assert_equal(targets_reg[i], targets_branchless_interleaved[i],
                 "Branchless interleaved targets mismatch");
    assert_equal(targets_reg[i], targets_branchless[i],
                 "Branchless targets mismatch");
  }

  std::cout << "Validation Succeeded..." << std::endl;
}
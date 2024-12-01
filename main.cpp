#include "common.hpp"

int main(int argc, char **argv) {

  Arguments args;
  try {
    args = get_arguments(argc, argv);

    std::cout << "Group size: " << args.group_size << '\n'
              << "Array size: " << args.array_size << '\n'
              << "Targets size: " << args.targets_size << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  }

  const int arraySize = args.array_size;
  const int targetsArraySize = args.targets_size;

  int *targets_reg = (int *)malloc(targetsArraySize * sizeof(int));
  int *targets_branchless = (int *)malloc(targetsArraySize * sizeof(int));
  int *targets_reg_interleaved = (int *)malloc(targetsArraySize * sizeof(int));
  int *targets_branchless_interleaved =
      (int *)malloc(targetsArraySize * sizeof(int));

  Search_ctx *ctx =
      create_ctx(arraySize, targetsArraySize, args.group_size);

  run_serial(ctx, targets_branchless, "branchless");
  run_serial(ctx, targets_reg, "regular");

  run_interleaved_coroutine(ctx, targets_branchless_interleaved, "branchless");
  run_interleaved_coroutine(ctx, targets_reg_interleaved, "regular");
  validate_target_arrays(targets_reg, targets_reg_interleaved,
                         targets_branchless, targets_branchless_interleaved,
                         ctx->targets, ctx->arr, targetsArraySize);

  free(targets_reg);
  free(targets_branchless_interleaved);
  free(ctx->targets);
  free(ctx->arr);
  free(targets_branchless);
  free(targets_reg_interleaved);

  return 0;
}

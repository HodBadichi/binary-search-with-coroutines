#include "common.hpp"

struct BSCoroutineHandler {
  struct promise_type {
    int value;
    BSCoroutineHandler get_return_object() {
      return {.h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_value(int v) { value = v; }
    void unhandled_exception() { std::terminate(); }
  };

  std::coroutine_handle<promise_type> h_;

  int get() { return h_.promise().value; }
  void resume() { this->h_.resume(); }
  bool done() { return this->h_.done(); }
};

BSCoroutineHandler branch_less_binary_search_coroutine(int *arr, int target,
                                                       const int arr_size) {
  int *base = arr, len = arr_size;
  while (len > 1) {
    int half = len / 2;
    __builtin_prefetch(&base[half - 1]);
    co_await std::suspend_always{};
    base += (base[half - 1] < target) * half; // will be replaced with a "cmov"
    len -= half;
  }
  co_return(base - arr);
}

BSCoroutineHandler binary_search_coroutine(int *arr, int target,
                                           const int arr_size) {
  int low = 0, high = arr_size - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;
    __builtin_prefetch(&arr[mid]);
    co_await std::suspend_always{};
    if (arr[mid] == target) {
      co_return mid;
    } else if (arr[mid] < target) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  co_return -1; // Not found
}

int binary_search(int *arr, int target, const int arr_size) {
  int low = 0, high = arr_size - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (arr[mid] == target) {
      return mid;
    } else if (arr[mid] < target) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return -1; // Not found
}

int branchless_binary_search(int *arr, int target, const int arr_size) {
  int *base = arr, len = arr_size;
  while (len > 1) {
    int half = len / 2;
    base += (base[half - 1] < target) * half; // will be replaced with a "cmov"
    len -= half;
  }
  return (base - arr);
}

void run_serial(Search_ctx *ctx, int *exp_targets, std::string search_type) {
  // Measure the time taken for binary search
  int (*binary_search_fn)(int *, int, const int); // Declare a function pointer
  std::string name;

  if (search_type == "regular") {
    binary_search_fn = binary_search;
    name = "regular binary search";
  } else if (search_type == "branchless") {
    binary_search_fn = branchless_binary_search;
    name = "branchless binary search";
  } else {
    std::cerr << search_type << std::endl;
    std::cerr << "Error: Illegal search type" << std::endl;
    exit(1);
  }

  auto start = chrono::high_resolution_clock::now();
  for (int i = 0; i < ctx->targets_arr_size; i++) {
    int result = binary_search_fn(ctx->arr, ctx->targets[i], ctx->arr_size);
    exp_targets[i] = result;
  }
  auto end = chrono::high_resolution_clock::now();

  chrono::duration<double> duration = end - start;
  print_results(ctx, name, duration);
}

void run_interleaved_coroutine(Search_ctx *ctx, int *exp_targets,
                               std::string search_type) {
  bool *finished = (bool *)malloc(ctx->group_size * sizeof(bool));
  BSCoroutineHandler *handles = (BSCoroutineHandler *)malloc(
      ctx->group_size * sizeof(BSCoroutineHandler));
  int *identifiers = (int *)malloc(ctx->group_size * sizeof(int));
  int value;
  int result;
  int not_done = ctx->group_size;
  int i = 0;
  int idx;

  BSCoroutineHandler (*binary_search_coroutine_fn)(int *arr, int target,
                                                   const int arr_size);
  std::string name;

  if (search_type == "regular") {
    binary_search_coroutine_fn = binary_search_coroutine;
    name = "regular interleaved co-routine binary search";
  } else if (search_type == "branchless") {
    binary_search_coroutine_fn = branch_less_binary_search_coroutine;
    name = "branchless interleaved co-routine binary search";
  } else {
    std::cerr << search_type << std::endl;
    std::cerr << "Error: Illegal search type" << std::endl;
    exit(1);
  }

  auto start = chrono::high_resolution_clock::now();

  for (; i < ctx->group_size; i++) {
    finished[i] = false;
    value = ctx->targets[i];

    handles[i] = binary_search_coroutine_fn(ctx->arr, value, ctx->arr_size);
    identifiers[i] = i;
  }

  int j = -1;
  while (true) {
    j++;
    if (j == ctx->group_size)
      j = 0;

    if (finished[j])
      continue;
    if (!handles[j].done()) {
      handles[j].resume();

    } else {
      result = handles[j].get();
      idx = identifiers[j];

      exp_targets[idx] = result;
      if (i < ctx->targets_arr_size) {
        value = ctx->targets[i];
        handles[j] = binary_search_coroutine_fn(ctx->arr, value, ctx->arr_size);
        identifiers[j] = i;
        i++;
      } else {
        not_done = not_done - 1;
        finished[j] = true;
        if (not_done == 0)
          break;
      }
    }
  }

  auto end = chrono::high_resolution_clock::now();

  free(finished);
  free(identifiers);
  free(handles);

  chrono::duration<double> duration = end - start;
  print_results(ctx, name, duration);
}

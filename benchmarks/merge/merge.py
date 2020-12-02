import iteration_utilities
import more_itertools
import toolz
import cytoolz
import heapq
import itertools

def bench_iu_merge(iterables, func=iteration_utilities.merge):
    iteration_utilities.consume(func(*iterables), None)

def bench_heapq_merge(iterables, func=heapq.merge):
    iteration_utilities.consume(func(*iterables), None)

def bench_builtin_sorted(iterables, func=sorted):
    return func(itertools.chain.from_iterable(iterables))

def bench_toolz_merge_sorted(iterables, func=toolz.merge_sorted):
    iteration_utilities.consume(func(*iterables), None)

def bench_cytoolz_merge_sorted(iterables, func=cytoolz.merge_sorted):
    iteration_utilities.consume(func(*iterables), None)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield (size, [list(range(size // 2)), list(range(size // 2))])

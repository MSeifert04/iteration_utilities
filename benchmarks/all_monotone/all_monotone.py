import iteration_utilities
import more_itertools

def bench_iu(iterable, func=iteration_utilities.all_monotone):
    return func(iterable)

def bench_mi(iterable, func=more_itertools.is_sorted):
    return func(iterable)

def bench_sorted(iterable, func=sorted):
    return func(iterable) == iterable

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, list(range(size))

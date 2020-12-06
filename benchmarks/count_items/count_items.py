import iteration_utilities
import more_itertools

def bench_iu_count_items(iterable, func=iteration_utilities.count_items):
    return func(iterable)

def bench_more_itertools_ilen(iterable, func=more_itertools.ilen):
    return func(iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [1] * size

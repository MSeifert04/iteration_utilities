import iteration_utilities
import more_itertools

def bench_iu_all_equal(iterable, func=iteration_utilities.all_equal):
    return func(iterable)

def bench_more_itertools_all_equal(iterable, func=more_itertools.all_equal):
    return func(iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [1] * size

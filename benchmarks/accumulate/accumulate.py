import iteration_utilities
import itertools

def bench_iu_accumulate(iterable, func=iteration_utilities.accumulate):
    iteration_utilities.consume(func(iterable), None)

def bench_itertools_accumulate(iterable, func=itertools.accumulate):
    iteration_utilities.consume(func(iterable), None)

def args_list_length():
    for exponent in range(2, 16):
        size = 2**exponent
        yield size, [1] * size

from operator import add

import iteration_utilities
import itertools
import toolz
import cytoolz


def bench_iu_accumulate(iterable, func=iteration_utilities.accumulate):
    iteration_utilities.consume(func(iterable, add), None)

def bench_itertools_accumulate(iterable, func=itertools.accumulate):
    iteration_utilities.consume(func(iterable, add), None)

def bench_toolz_accumulate(iterable, func=toolz.accumulate):
    iteration_utilities.consume(func(add, iterable), None)

def bench_cytoolz_accumulate(iterable, func=cytoolz.accumulate):
    iteration_utilities.consume(func(add, iterable), None)

def args_list_length():
    for exponent in range(2, 20):
        size = 2**exponent
        yield size, [1] * size

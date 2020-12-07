import iteration_utilities
import toolz
import cytoolz
import more_itertools
import pydash

def bench_iu_unique_everseen(iterable, func=iteration_utilities.unique_everseen):
    iteration_utilities.consume(func(iterable), None)

def bench_more_itertools_unique_everseen(iterable, func=more_itertools.unique_everseen):
    iteration_utilities.consume(func(iterable), None)

def bench_toolz_unique(iterable, func=toolz.unique):
    iteration_utilities.consume(func(iterable), None)

def bench_cytoolz_unique(iterable, func=cytoolz.unique):
    iteration_utilities.consume(func(iterable), None)

def bench_pd(iterable, func=pydash.uniq):
    func(iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [0] * size

import iteration_utilities
import more_itertools
import toolz
import cytoolz
import pydash

def bench_iu_intersperse(iterable, func=iteration_utilities.intersperse):
    iteration_utilities.consume(func(iterable, 2), None)

def bench_more_itertools_intersperse(iterable, func=more_itertools.intersperse):
    iteration_utilities.consume(func(2, iterable), None)

def bench_toolz_interpose(iterable, func=toolz.interpose):
    iteration_utilities.consume(func(2, iterable), None)

def bench_cytoolz_interpose(iterable, func=cytoolz.interpose):
    iteration_utilities.consume(func(2, iterable), None)

def bench_pd(iterable, func=pydash.intersperse):
    func(iterable, 2)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [i % 10 for i in range(size)]

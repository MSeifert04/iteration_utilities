import iteration_utilities
import more_itertools
import toolz
import cytoolz
import pydash

def bench_iu_roundrobin(iterables, func=iteration_utilities.roundrobin):
    iteration_utilities.consume(func(*iterables), None)

def bench_more_itertools_roundrobin(iterables, func=more_itertools.roundrobin):
    iteration_utilities.consume(func(*iterables), None)

def bench_toolz_interleave(iterables, func=toolz.interleave):
    iteration_utilities.consume(func(iterables), None)

def bench_cytoolz_interleave(iterables, func=cytoolz.interleave):
    iteration_utilities.consume(func(iterables), None)

def bench_pd(iterables, func=pydash.interleave):
    func(*iterables)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [[0] * (size // 2), [1] * (size // 2)]

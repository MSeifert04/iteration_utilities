import iteration_utilities
import more_itertools
import toolz
import cytoolz

def bench_iu_grouper(iterable, func=iteration_utilities.grouper):
    iteration_utilities.consume(func(iterable, 50), None)

def bench_more_itertools_grouper(iterable, func=more_itertools.grouper):
    iteration_utilities.consume(func(iterable, 50), None)

def bench_more_itertools_chunked(iterable, func=more_itertools.chunked):
    iteration_utilities.consume(func(iterable, 50), None)

def bench_toolz_partition(iterable, func=toolz.partition):
    iteration_utilities.consume(func(50, iterable), None)

def bench_cytoolz_partition(iterable, func=cytoolz.partition):
    iteration_utilities.consume(func(50, iterable), None)

def bench_toolz_partition_all(iterable, func=toolz.partition_all):
    iteration_utilities.consume(func(50, iterable), None)

def bench_cytoolz_partition_all(iterable, func=cytoolz.partition_all):
    iteration_utilities.consume(func(50, iterable), None)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [i % 10 for i in range(size)]

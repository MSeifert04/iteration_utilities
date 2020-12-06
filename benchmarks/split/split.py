import iteration_utilities
import more_itertools

def bench_iu_split(iterable, func=iteration_utilities.split):
    iteration_utilities.consume(func(iterable, lambda x: x % 5 == 0), None)

def bench_more_itertools_split_at(iterable, func=more_itertools.split_at):
    iteration_utilities.consume(func(iterable, lambda x: x % 5 == 0), None)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, list(range(size))

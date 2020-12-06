import iteration_utilities
import more_itertools

def bench_iu_unique_justseen(iterable, func=iteration_utilities.unique_justseen):
    iteration_utilities.consume(func(iterable), None)

def bench_more_itertools_unique_justseen(iterable, func=more_itertools.unique_justseen):
    iteration_utilities.consume(func(iterable), None)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, sorted(range(size))

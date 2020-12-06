import iteration_utilities
import toolz
import cytoolz
import more_itertools

def bench_iu_successive(iterable, func=iteration_utilities.successive):
    iteration_utilities.consume(func(iterable, 50), None)

def bench_toolz_sliding_window(iterable, func=toolz.sliding_window):
    iteration_utilities.consume(func(50, iterable), None)

def bench_cytoolz_sliding_window(iterable, func=cytoolz.sliding_window):
    iteration_utilities.consume(func(50, iterable), None)

def bench_more_itertools_windowed(iterable, func=more_itertools.windowed):
    iteration_utilities.consume(func(iterable, 50), None)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [0] * size

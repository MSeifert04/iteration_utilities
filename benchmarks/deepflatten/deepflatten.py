import iteration_utilities
import more_itertools
import pydash

def bench_iu(iterable, func=iteration_utilities.deepflatten):
    return iteration_utilities.consume(func(iterable), None)

def bench_mi(iterable, func=more_itertools.collapse):
    return iteration_utilities.consume(func(iterable), None)

def bench_pd(iterable, func=pydash.flatten_deep):
    return func(iterable)

def args_list_length():
    for exponent in range(0, 15):
        size = 2**exponent
        yield size * 4, [[[(0, ) * 2]] * 2] * size

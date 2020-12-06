import iteration_utilities
import toolz
import cytoolz

def bench_iu_all_distinct(iterable, func=iteration_utilities.all_distinct):
    return func(iterable)

def bench_toolz_isdistinct(iterable, func=toolz.isdistinct):
    return func(iterable)

def bench_cytoolz_isdistinct(iterable, func=cytoolz.isdistinct):
    return func(iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, list(range(size))

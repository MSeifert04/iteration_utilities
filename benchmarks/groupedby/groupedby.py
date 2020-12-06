import iteration_utilities
import toolz
import cytoolz

def bench_iu_groupedby(iterable, func=iteration_utilities.groupedby):
    return func(iterable, iteration_utilities.return_identity)

def bench_toolz_groupby(iterable, func=toolz.groupby):
    return func(iteration_utilities.return_identity, iterable)

def bench_cytoolz_groupby(iterable, func=cytoolz.groupby):
    return func(iteration_utilities.return_identity, iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [i % 10 for i in range(size)]

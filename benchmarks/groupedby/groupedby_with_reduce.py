import operator

import iteration_utilities
import toolz
import cytoolz

def bench_iu_groupedby(iterable, func=iteration_utilities.groupedby):
    return func(iterable, iteration_utilities.return_identity, reduce=operator.add)

def bench_toolz_reduceby(iterable, func=toolz.reduceby):
    return func(iteration_utilities.return_identity, operator.add, iterable)

def bench_cytoolz_reduceby(iterable, func=cytoolz.reduceby):
    return func(iteration_utilities.return_identity, operator.add, iterable)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, [i % 10 for i in range(size)]

import iteration_utilities
import toolz
import cytoolz
import more_itertools

def bench_iu_applyfunc(n, func=iteration_utilities.applyfunc):
    iteration_utilities.consume(func(iteration_utilities.return_True, 1), n)

def bench_more_itertools_iterate(n, func=more_itertools.iterate):
    iteration_utilities.consume(func(iteration_utilities.return_True, 1), n)

def bench_toolz_iterate(n, func=toolz.iterate):
    iteration_utilities.consume(func(iteration_utilities.return_True, 1), n)

def bench_cytoolz_iterate(n, func=cytoolz.iterate):
    iteration_utilities.consume(func(iteration_utilities.return_True, 1), n)

def args_list_length():
    for exponent in range(2, 18):
        size = 2**exponent
        yield size, size

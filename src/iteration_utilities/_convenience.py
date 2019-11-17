# Licensed under Apache License Version 2.0 - see LICENSE

from iteration_utilities import constant, nth

__all__ = ["return_True", "return_False", "return_None", "first", "second", "third", "last"]

return_True = constant(True)
return_False = constant(False)
return_None = constant(None)

first = nth(0)
second = nth(1)
third = nth(2)
last = nth(-1)

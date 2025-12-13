# Licensed under Apache License Version 2.0 - see LICENSE

import copy

import pytest

from iteration_utilities import partial


def test_placeholder_new():
    with pytest.raises(TypeError, match=r"_PlaceholderType\.__new__` takes no arguments"):
        type(partial._)(1)
    with pytest.raises(TypeError, match=r"_PlaceholderType\.__new__` takes no arguments"):
        type(partial._)(a=1)


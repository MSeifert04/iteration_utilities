# 3rd party
import pytest
from contextlib import contextmanager

# Memory leak with version 3.0.2 which makes it damn hard to test for memory
# leaks on CI. Remove this if noone uses 3.0.2 anymore!
# https://github.com/pytest-dev/pytest/issues/1965
if pytest.__version__ not in ('3.0.2', '3.0.3'):
    pytest_raises = pytest.raises
else:
    @contextmanager
    def pytest_raises(exc):
        try:
            yield
        except exc:
            pass
        else:
            raise TypeError('did not raise {0!s}'.format(exc))

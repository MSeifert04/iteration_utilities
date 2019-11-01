import pickle

import pytest


@pytest.fixture(scope="module", params=range(pickle.HIGHEST_PROTOCOL + 1))
def protocol(request):
    """Returns all available pickle protocols.

    This avoids needing to parametrize all test functions manually."""
    yield request.param

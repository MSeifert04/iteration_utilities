# 3rd party
import pytest

# Monkeypatch is not needed because the issue was just some cyclic references
# that are resolved by adding "gc.collect()" in the memoryleak helper.
pytest_raises = pytest.raises

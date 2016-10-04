# Built-ins
import sys

__all__ = ['PY2', 'PY3', 'PY34']

PY2 = sys.version_info.major == 2
PY3 = sys.version_info.major == 3
PY34 = PY3 and sys.version_info.minor >= 4

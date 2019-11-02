#!/bin/bash
set -e -x

python -m pip install pip --upgrade
python -m pip install setuptools wheel pytest

python -m pip install /io -vv

python -m pytest /io/tests

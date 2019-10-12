#ifndef PYIU_PLACEHOLDER_H
#define PYIU_PLACEHOLDER_H

#include <Python.h>
#include "helpercompat.h"

extern PyObject PlaceholderStruct;
extern PyTypeObject Placeholder_Type;
#define PYIU_Placeholder (&PlaceholderStruct)

#endif

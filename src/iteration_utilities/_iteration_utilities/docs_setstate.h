#ifndef PYIU_DOCSSETSTATE_H
#define PYIU_DOCSSETSTATE_H

#include <Python.h>

PyDoc_STRVAR(PYIU_setstate_doc,
    "__setstate__(state, /)\n"
    "--\n\n"
    "Set state for unpickling. "
    "The `state` argument must be `tuple`-like.\n"
);

#endif

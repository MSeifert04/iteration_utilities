#ifndef PYIU_HELPERCOMPAT_H
#define PYIU_HELPERCOMPAT_H

#if PY_MAJOR_VERSION == 2
    #define Py_RETURN_NOTIMPLEMENTED \
        return PyErr_SetString(PyExc_TypeError, "not implemented."), NULL

    #define Py_UNUSED(name) _unused_ ## name

    #if defined(_MSC_VER)
        #define inline __inline
    #endif
#endif

#endif

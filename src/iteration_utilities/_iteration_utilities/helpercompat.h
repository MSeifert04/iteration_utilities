#ifndef PYIU_HELPERCOMPAT_H
#define PYIU_HELPERCOMPAT_H

#ifdef PYPY_VERSION
  #define PYIU_PYPY 1
  #define PYIU_CPYTHON 0
#else
  #define PYIU_PYPY 0
  #define PYIU_CPYTHON 1
#endif

#if PYIU_PYPY
  // Both were added in PyPy3.6 7.2.ß.
  #ifndef Py_RETURN_NOTIMPLEMENTED
    #define Py_RETURN_NOTIMPLEMENTED return Py_INCREF(Py_NotImplemented), Py_NotImplemented
  #endif
  #ifndef Py_UNUSED
    #define Py_UNUSED(name) _unused_ ## name
  #endif
#endif

#if PYIU_CPYTHON && PY_MAJOR_VERSION == 2
    #define Py_RETURN_NOTIMPLEMENTED \
        return PyErr_SetString(PyExc_TypeError, "not implemented."), NULL

    #define Py_UNUSED(name) _unused_ ## name

    #if defined(_MSC_VER)
        #define inline __inline
    #endif
#endif

#define PyIU_USE_FASTCALL PYIU_CPYTHON && PY_MAJOR_VERSION == 3 && (PY_MINOR_VERSION == 6 || PY_MINOR_VERSION == 7)
#define PyIU_USE_VECTORCALL PYIU_CPYTHON && PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8

#endif

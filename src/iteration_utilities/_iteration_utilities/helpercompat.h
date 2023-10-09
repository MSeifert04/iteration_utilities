#ifndef PYIU_HELPERCOMPAT_H
#define PYIU_HELPERCOMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

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

#if PYIU_PYPY
  // Taken from PyObject_HEAD_INIT implementation (it's a bit hacky...)
  #define PYIU_CREATE_SINGLETON_INSTANCE(type) { 1, 0, &type }
#else
  #if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 12
    #define PYIU_CREATE_SINGLETON_INSTANCE(type) { _PyObject_EXTRA_INIT 1, &type }
  #elif PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 12
    #define PYIU_CREATE_SINGLETON_INSTANCE(type) { _PyObject_EXTRA_INIT { _Py_IMMORTAL_REFCNT }, &type }
  #else
    // It seems like they will remove the _PyObject_EXTRA_INIT (not present on master of CPython anymore)
    // it could still be subject to change but compare to the definition for _Py_NotImplementedStruct or _Py_NoneStruct
    #define PYIU_CREATE_SINGLETON_INSTANCE(type) { { _Py_IMMORTAL_REFCNT }, &type }
  #endif
#endif

#define PyIU_USE_FASTCALL (PYIU_CPYTHON && PY_MAJOR_VERSION == 3 && (PY_MINOR_VERSION == 6 || PY_MINOR_VERSION == 7))
#define PyIU_USE_UNDERSCORE_VECTORCALL (PYIU_CPYTHON && PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8)
#define PyIU_USE_VECTORCALL (PYIU_CPYTHON && ((PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 8) || PY_MAJOR_VERSION >= 4))
#define PyIU_USE_BUILTIN_MODULE_ADDTYPE ((PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 9) || PY_MAJOR_VERSION >= 4)

#ifdef Py_IS_TYPE
  #define PyIU_USE_BUILTIN_IS_TYPE 1
#else
  #define PyIU_USE_BUILTIN_IS_TYPE 0
#endif

#ifdef __cplusplus
}
#endif

#endif

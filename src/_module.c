/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

#include <Python.h>
#include <structmember.h>

#include "_helper.c"
#include "_globaldocs.c"
#include "_seen.c"
#include "_itemidxkey.c"
#include "_isx.c"
#include "_returnx.c"
#include "_mathematical.c"

#include "chained.c"
#include "complement.c"
#include "const.c"
#include "flip.c"
#include "nth.c"

#include "argminmax.c"
#include "alldistinct.c"
#include "allequal.c"
#include "allmonotone.c"
#include "countitems.c"
#include "dotproduct.c"
#include "groupedby.c"
#include "minmax.c"
#include "one.c"
#include "partition.c"

#include "accumulate.c"
#include "applyfunc.c"
#include "clamp.c"
#include "deepflatten.c"
#include "duplicates.c"
#include "grouper.c"
#include "intersperse.c"
#include "iterexcept.c"
#include "merge.c"
#include "roundrobin.c"
#include "sideeffect.c"
#include "split.c"
#include "successive.c"
#include "tabulate.c"
#include "uniqueever.c"
#include "uniquejust.c"

/* Method definition object for this extension, these argumens mean:
   ml_name: The name of the method
   ml_meth: Function pointer to the method implementation
   ml_flags: Flags indicating special features of this method, such as
             accepting arguments, accepting keyword arguments, being a
             class method, or being a static method of a class.
   ml_doc:  Contents of this method's docstring.
   */

static PyMethodDef PyIU_methods[] = {

    {"is_None",     (PyCFunction)PyIU_IsNone,     METH_O,  PyIU_IsNone_doc},
    {"is_not_None", (PyCFunction)PyIU_IsNotNone,  METH_O,  PyIU_IsNotNone_doc},
    {"is_even",     (PyCFunction)PyIU_IsEven,     METH_O,  PyIU_IsEven_doc},
    {"is_odd",      (PyCFunction)PyIU_IsOdd,      METH_O,  PyIU_IsOdd_doc},
    {"is_iterable", (PyCFunction)PyIU_IsIterable, METH_O,  PyIU_IsIterable_doc},

    {"square",     (PyCFunction)PyIU_MathSquare,     METH_O,       PyIU_MathSquare_doc},
    {"double",     (PyCFunction)PyIU_MathDouble,     METH_O,       PyIU_MathDouble_doc},
    {"reciprocal", (PyCFunction)PyIU_MathReciprocal, METH_O,       PyIU_MathReciprocal_doc},
    {"radd",       (PyCFunction)PyIU_MathRadd,       METH_VARARGS, PyIU_MathRadd_doc},
    {"rsub",       (PyCFunction)PyIU_MathRsub,       METH_VARARGS, PyIU_MathRsub_doc},
    {"rmul",       (PyCFunction)PyIU_MathRmul,       METH_VARARGS, PyIU_MathRmul_doc},
    {"rdiv",       (PyCFunction)PyIU_MathRdiv,       METH_VARARGS, PyIU_MathRdiv_doc},
    {"rfdiv",      (PyCFunction)PyIU_MathRfdiv,      METH_VARARGS, PyIU_MathRfdiv_doc},
    {"rpow",       (PyCFunction)PyIU_MathRpow,       METH_VARARGS, PyIU_MathRpow_doc},
    {"rmod",       (PyCFunction)PyIU_MathRmod,       METH_VARARGS, PyIU_MathRmod_doc},

    {"return_identity",  (PyCFunction)PyIU_ReturnIdentity, METH_O,                       PyIU_ReturnIdentity_doc},
    {"return_first_arg", (PyCFunction)PyIU_ReturnFirstArg, METH_VARARGS | METH_KEYWORDS, PyIU_ReturnFirstArg_doc},
    {"return_called",    (PyCFunction)PyIU_ReturnCalled,   METH_O,                       PyIU_ReturnCalled_doc},

    {"argmin",       (PyCFunction)PyIU_Argmin,      METH_VARARGS | METH_KEYWORDS, PyIU_Argmin_doc},
    {"argmax",       (PyCFunction)PyIU_Argmax,      METH_VARARGS | METH_KEYWORDS, PyIU_Argmax_doc},
    {"all_distinct", (PyCFunction)PyIU_AllDistinct, METH_O,                       PyIU_AllDistinct_doc},
    {"all_equal",    (PyCFunction)PyIU_AllEqual,    METH_O,                       PyIU_AllEqual_doc},
    {"all_monotone", (PyCFunction)PyIU_Monotone,    METH_VARARGS | METH_KEYWORDS, PyIU_Monotone_doc},
    {"count_items",  (PyCFunction)PyIU_Count,       METH_VARARGS | METH_KEYWORDS, PyIU_Count_doc},
    {"dotproduct",   (PyCFunction)PyIU_DotProduct,  METH_VARARGS,                 PyIU_DotProduct_doc},
    {"groupedby",    (PyCFunction)PyIU_Groupby,     METH_VARARGS | METH_KEYWORDS, PyIU_Groupby_doc},
    {"minmax",       (PyCFunction)PyIU_MinMax,      METH_VARARGS | METH_KEYWORDS, PyIU_MinMax_doc},
    {"one",          (PyCFunction)PyIU_One,         METH_O,                       PyIU_One_doc},
    {"partition",    (PyCFunction)PyIU_Partition,   METH_VARARGS | METH_KEYWORDS, PyIU_Partition_doc},

    {NULL, NULL}
};

/* Names for pre-defined instances. */
PyDoc_STRVAR(PyIU_returnTrue_name, "return_True");
PyDoc_STRVAR(PyIU_returnFalse_name, "return_False");
PyDoc_STRVAR(PyIU_returnNone_name, "return_None");
PyDoc_STRVAR(PyIU_ReduceFirst_name, "first");
PyDoc_STRVAR(PyIU_ReduceSecond_name, "second");
PyDoc_STRVAR(PyIU_ReduceThird_name, "third");
PyDoc_STRVAR(PyIU_ReduceLast_name, "last");

/* Name and docstring of C-module. */
PyDoc_STRVAR(PyIU_module_name, "_cfuncs");
PyDoc_STRVAR(PyIU_module_doc, "API: C Functions\n----------------");

#if PY_MAJOR_VERSION >= 3
  /* Module definition */
  static struct PyModuleDef PyIU_module = {

    PyModuleDef_HEAD_INIT,

    PyIU_module_name,         /* module name */
    PyIU_module_doc,          /* module docstring */
    -1,                       /* API version */
    PyIU_methods,             /* module methods */

    NULL, NULL, NULL, NULL    /* Sentinel */
  };

  /* Module initialization */
  PyMODINIT_FUNC
  PyInit__cfuncs(void)
  {
#else
  void
  init_cfuncs(void)
  {
#endif

    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;
    PyObject *PyIU_returnTrue, *PyIU_returnFalse, *PyIU_returnNone;
    PyObject *PyIU_ReduceFirst, *PyIU_ReduceSecond, *PyIU_ReduceThird, *PyIU_ReduceLast;
    Py_ssize_t minus_one = -1;  /* no idea why this is needed but -1 in call doesn't work */

    /* Classes avaiable in module. */
    PyTypeObject *typelist[] = {
        &PyIUType_ItemIdxKey,
        &PyIUType_Seen,

        &PyIUType_Chained,
        &PyIUType_Complement,
        &PyIUType_Constant,
        &PyIUType_Flip,
        &PyIUType_Nth,

        &PyIUType_Accumulate,
        &PyIUType_Applyfunc,
        &PyIUType_Clamp,
        &PyIUType_DeepFlatten,
        &PyIUType_Duplicates,
        &PyIUType_Grouper,
        &PyIUType_Intersperse,
        &PyIUType_Iterexcept,
        &PyIUType_Merge,
        &PyIUType_Roundrobin,
        &PyIUType_Sideeffects,
        &PyIUType_Split,
        &PyIUType_Successive,
        &PyIUType_Tabulate,
        &PyIUType_UniqueEver,
        &PyIUType_UniqueJust,
        NULL
    };

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&PyIU_module);
#else
    m = Py_InitModule3(PyIU_module_name, PyIU_methods, PyIU_module_doc);
#endif

    if (m != NULL) {

        /* Add classes to the module but only use the name starting after the first
           occurence of ".".
           */
        for (i=0 ; typelist[i] != NULL ; i++) {
            if (PyType_Ready(typelist[i]) < 0)
#if PY_MAJOR_VERSION >= 3
                return m;
#else
                return;
#endif
            name = strchr(typelist[i]->tp_name, '.');
            assert (name != NULL);
            Py_INCREF(typelist[i]);
            PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
        }

        /* Add pre-defined instances. */
        PyIU_returnTrue = constant_new(&PyIUType_Constant, Py_BuildValue("(O)", Py_True), NULL);
        PyModule_AddObject(m, PyIU_returnTrue_name, PyIU_returnTrue);
        PyIU_returnFalse = constant_new(&PyIUType_Constant, Py_BuildValue("(O)", Py_False), NULL);
        PyModule_AddObject(m, PyIU_returnFalse_name, PyIU_returnFalse);
        PyIU_returnNone = constant_new(&PyIUType_Constant, Py_BuildValue("(O)", Py_None), NULL);
        PyModule_AddObject(m, PyIU_returnNone_name, PyIU_returnNone);

        PyIU_ReduceFirst = nth_new(&PyIUType_Nth, Py_BuildValue("(n)", 0), NULL);
        PyModule_AddObject(m, PyIU_ReduceFirst_name, PyIU_ReduceFirst);
        PyIU_ReduceSecond = nth_new(&PyIUType_Nth, Py_BuildValue("(n)", 1), NULL);
        PyModule_AddObject(m, PyIU_ReduceSecond_name, PyIU_ReduceSecond);
        PyIU_ReduceThird = nth_new(&PyIUType_Nth, Py_BuildValue("(n)", 2), NULL);
        PyModule_AddObject(m, PyIU_ReduceThird_name, PyIU_ReduceThird);
        PyIU_ReduceLast = nth_new(&PyIUType_Nth, Py_BuildValue("(n)", minus_one), NULL);
        PyModule_AddObject(m, PyIU_ReduceLast_name, PyIU_ReduceLast);
    }

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}

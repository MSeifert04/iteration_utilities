#include <Python.h>
#include <structmember.h>

#include "allhelper.c"
#include "isx.c"
#include "returnx.c"
#include "mathematical.c"
#include "functionscomplement.c"
#include "functionscompose.c"
#include "functionsconst.c"
#include "functionsflip.c"
#include "functionsnth.c"
#include "reduceargminmax.c"
#include "reducealldistinct.c"
#include "reduceallequal.c"
#include "reduceminmax.c"
#include "reducegroupby.c"
#include "reduceilen.c"
#include "reduceone.c"
#include "reducepartition.c"
#include "reducequantify.c"
#include "recipesuniquejust.c"
#include "recipesuniqueever.c"
#include "recipesaccumulate.c"
#include "recipesapplyfunc.c"
#include "recipesgrouper.c"
#include "recipesintersperse.c"
#include "recipesiterexcept.c"
#include "recipessplit.c"
#include "recipessuccessive.c"
#include "recipesroundrobin.c"
#include "recipesmerge.c"


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef PyIU_methods[] = {

    {"is_None", (PyCFunction)PyIU_IsNone, METH_O, PyIU_IsNone_doc},
    {"is_not_None", (PyCFunction)PyIU_IsNotNone, METH_O, PyIU_IsNotNone_doc},
    {"is_even", (PyCFunction)PyIU_IsEven, METH_O, PyIU_IsEven_doc},
    {"is_odd", (PyCFunction)PyIU_IsOdd, METH_O, PyIU_IsOdd_doc},
    {"is_iterable", (PyCFunction)PyIU_IsIterable, METH_O, PyIU_IsIterable_doc},

    {"square", (PyCFunction)PyIU_MathSquare, METH_O, PyIU_MathSquare_doc},
    {"double", (PyCFunction)PyIU_MathDouble, METH_O, PyIU_MathDouble_doc},
    {"one_over", (PyCFunction)PyIU_MathOneOver, METH_O, PyIU_MathOneOver_doc},
    {"radd", (PyCFunction)PyIU_MathRadd, METH_VARARGS, PyIU_MathRadd_doc},
    {"rsub", (PyCFunction)PyIU_MathRsub, METH_VARARGS, PyIU_MathRsub_doc},
    {"rmul", (PyCFunction)PyIU_MathRmul, METH_VARARGS, PyIU_MathRmul_doc},
    {"rdiv", (PyCFunction)PyIU_MathRdiv, METH_VARARGS, PyIU_MathRdiv_doc},
    {"rpow", (PyCFunction)PyIU_MathRpow, METH_VARARGS, PyIU_MathRpow_doc},

    {"return_identity", (PyCFunction)PyIU_ReturnIdentity, METH_O, PyIU_ReturnIdentity_doc},
    {"return_first_arg", (PyCFunction)PyIU_ReturnFirstArg, METH_VARARGS | METH_KEYWORDS, PyIU_ReturnFirstArg_doc},
    {"return_called", (PyCFunction)PyIU_ReturnCalled, METH_O, PyIU_ReturnCalled_doc},

    {"argmin", (PyCFunction)reduce_argmin, METH_VARARGS | METH_KEYWORDS, reduce_argmin_doc},
    {"argmax", (PyCFunction)reduce_argmax, METH_VARARGS | METH_KEYWORDS, reduce_argmax_doc},
    {"all_distinct", (PyCFunction)reduce_alldistinct, METH_O, reduce_alldistinct_doc},
    {"all_equal", (PyCFunction)reduce_allequal, METH_O, reduce_allequal_doc},
    {"minmax", (PyCFunction)reduce_minmax, METH_VARARGS | METH_KEYWORDS, reduce_minmax_doc},
    {"one", (PyCFunction)reduce_one, METH_O, reduce_one_doc},
    {"groupby2", (PyCFunction)reduce_groupby, METH_VARARGS | METH_KEYWORDS, reduce_groupby_doc},
    {"quantify", (PyCFunction)reduce_quantify, METH_VARARGS | METH_KEYWORDS, reduce_quantify_doc},
    {"partition", (PyCFunction)recipes_partition, METH_VARARGS | METH_KEYWORDS, recipes_partition_doc},
    {"ilen", (PyCFunction)reduce_ilen, METH_VARARGS | METH_KEYWORDS, reduce_ilen_doc},

    {NULL, NULL}
};

// Names for pre-defined instances.
PyDoc_STRVAR(PyIU_returnTrue_name, "return_True");
PyDoc_STRVAR(PyIU_returnFalse_name, "return_False");
PyDoc_STRVAR(PyIU_returnNone_name, "return_None");
PyDoc_STRVAR(returnx_nthFirst_name, "first");
PyDoc_STRVAR(returnx_nthSecond_name, "second");
PyDoc_STRVAR(returnx_nthThird_name, "third");
PyDoc_STRVAR(returnx_nthLast_name, "last");

// Name and docstring of C-module
PyDoc_STRVAR(PyIU_module_name, "_cfuncs");
PyDoc_STRVAR(PyIU_module_doc, "C Functions\n^^^^^^^^^^^^^^^^");

#if PY_MAJOR_VERSION >= 3
  //Module definition
  static struct PyModuleDef PyIU_module = {

    PyModuleDef_HEAD_INIT,

    PyIU_module_name,         /* module name */
    PyIU_module_doc,          /* module docstring */
    -1,                       /* API version */
    PyIU_methods,             /* module methods */

    NULL, NULL, NULL, NULL    /* Sentinel */
  };

  //Module initialization
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
    PyObject *returnx_returnFirst, *returnx_returnSecond, *returnx_returnThird;
    PyObject *returnx_returnLast;
    Py_ssize_t minus_one = -1;  // no idea why this is needed but -1 in call doesn't work

    // Classes avaiable in module
    PyTypeObject *typelist[] = {
        &functions_complement_type,
        &functions_compose_type,
        &functions_constant_type,
        &functions_flip_type,
        &functions_nth_type,
        &recipes_accumulate_type,
        &recipes_applyfunc_type,
        &recipes_grouper_type,
        &recipes_intersperse_type,
        &recipes_iterexcept_type,
        &recipes_merge_type,
        &recipes_roundrobin_type,
        &recipes_split_type,
        &recipes_successive_type,
        &recipes_uniqueever_type,
        &recipes_uniquejust_type,
        NULL
    };

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&PyIU_module);
#else
    m = Py_InitModule3(PyIU_module_name, PyIU_methods, PyIU_module_doc);
#endif

    if (m == NULL)
        return NULL;

    // Add classes to the module but only use the name starting after the first
    // occurence of ".".
    for (i=0 ; typelist[i] != NULL ; i++) {
        if (PyType_Ready(typelist[i]) < 0)
            return NULL;
        name = strchr(typelist[i]->tp_name, '.');
        assert (name != NULL);
        Py_INCREF(typelist[i]);
        PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
    }

    // Add pre-defined instances.
    PyIU_returnTrue = functions_constant_new(&functions_constant_type,
                             Py_BuildValue("(O)", Py_True), NULL);
    PyModule_AddObject(m, PyIU_returnTrue_name, PyIU_returnTrue);
    PyIU_returnFalse = functions_constant_new(&functions_constant_type,
                              Py_BuildValue("(O)", Py_False), NULL);
    PyModule_AddObject(m, PyIU_returnFalse_name, PyIU_returnFalse);
    PyIU_returnNone = functions_constant_new(&functions_constant_type,
                             Py_BuildValue("(O)", Py_None), NULL);
    PyModule_AddObject(m, PyIU_returnNone_name, PyIU_returnNone);

    returnx_returnFirst = functions_nth_new(&functions_nth_type,
                              Py_BuildValue("(n)", 0), NULL);
    PyModule_AddObject(m, returnx_nthFirst_name, returnx_returnFirst);
    returnx_returnSecond = functions_nth_new(&functions_nth_type,
                              Py_BuildValue("(n)", 1), NULL);
    PyModule_AddObject(m, returnx_nthSecond_name, returnx_returnSecond);
    returnx_returnThird = functions_nth_new(&functions_nth_type,
                              Py_BuildValue("(n)", 2), NULL);
    PyModule_AddObject(m, returnx_nthThird_name, returnx_returnThird);
    returnx_returnLast = functions_nth_new(&functions_nth_type,
                              Py_BuildValue("(n)", minus_one), NULL);
    PyModule_AddObject(m, returnx_nthLast_name, returnx_returnLast);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
#include <Python.h>
#include <structmember.h>
#include "isx.c"
#include "returnx.c"
#include "mathematical.c"
#include "functionscomplement.c"
#include "reducealldistinct.c"
#include "reduceallequal.c"
#include "reduceminmax.c"
#include "reducefirst.c"
#include "reducegroupby.c"
#include "reduceilen.c"
#include "reducelast.c"
#include "reducenth.c"
#include "reduceone.c"
#include "reducepartition.c"
#include "reducequantify.c"
#include "reducesecond.c"
#include "recipesuniquejust.c"
#include "recipesuniqueever.c"
#include "recipesaccumulate.c"
#include "recipesapplyfunc.c"
#include "recipesgrouper.c"
#include "recipesintersperse.c"
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
static PyMethodDef
iterationutils_methods[] = {

    {"is_None",
     (PyCFunction)isx_IsNone,
     METH_O,
     isx_IsNone_doc},

    {"is_not_None",
     (PyCFunction)isx_IsNotNone,
     METH_O,
     isx_IsNotNone_doc},

    {"is_even",
     (PyCFunction)isx_IsEven,
     METH_O,
     isx_IsEven_doc},

    {"is_odd",
     (PyCFunction)isx_IsOdd,
     METH_O,
     isx_IsOdd_doc},

    {"is_iterable",
     (PyCFunction)isx_IsIterable,
     METH_O,
     isx_IsIterable_doc},

    {"square",
     (PyCFunction)mathematical_square,
     METH_O,
     mathematical_square_doc},

    {"one_over",
     (PyCFunction)mathematical_oneover,
     METH_O,
     mathematical_oneover_doc},

    {"return_True",
     (PyCFunction)returnx_returnTrue,
     METH_VARARGS | METH_KEYWORDS,
     returnx_returnTrue_doc},

    {"return_False",
     (PyCFunction)returnx_returnFalse,
     METH_VARARGS | METH_KEYWORDS,
     returnx_returnFalse_doc},

    {"return_None",
     (PyCFunction)returnx_returnNone,
     METH_VARARGS | METH_KEYWORDS,
     returnx_returnNone_doc},

    {"return_called",
     (PyCFunction)returnx_returnCallResult,
     METH_O,
     returnx_returnCallResult_doc},

    {"return_identity",
     (PyCFunction)returnx_returnIt,
     METH_O,
     returnx_returnIt_doc},

    {"return_first_positional_argument",
     (PyCFunction)returnx_returnFirstPositionalArgument,
     METH_VARARGS | METH_KEYWORDS,
     returnx_returnFirstPositionalArgument_doc},

    {"all_distinct",
     (PyCFunction)reduce_alldistinct,
     METH_O,
     reduce_alldistinct_doc},

    {"all_equal",
     (PyCFunction)reduce_allequal,
     METH_O,
     reduce_allequal_doc},

    {"minmax",
     (PyCFunction)reduce_minmax,
     METH_VARARGS | METH_KEYWORDS,
     reduce_minmax_doc},

    {"first",
     (PyCFunction)reduce_first,
     METH_VARARGS | METH_KEYWORDS,
     reduce_first_doc},

    {"second",
     (PyCFunction)reduce_second,
     METH_VARARGS | METH_KEYWORDS,
     reduce_second_doc},

    {"last",
     (PyCFunction)reduce_last,
     METH_VARARGS | METH_KEYWORDS,
     reduce_last_doc},

    {"one",
     (PyCFunction)reduce_one,
     METH_O,
     reduce_one_doc},

    {"groupby2",
     (PyCFunction)reduce_groupby,
     METH_VARARGS | METH_KEYWORDS,
     reduce_groupby_doc},

    {"quantify",
     (PyCFunction)reduce_quantify,
     METH_VARARGS | METH_KEYWORDS,
     reduce_quantify_doc},

    {"partition",
     (PyCFunction)recipes_partition,
     METH_VARARGS | METH_KEYWORDS,
     recipes_partition_doc},

    {"nth",
     (PyCFunction)reduce_nth,
     METH_VARARGS | METH_KEYWORDS,
     reduce_nth_doc},

    {"ilen",
     (PyCFunction)reduce_ilen,
     METH_VARARGS | METH_KEYWORDS,
     reduce_ilen_doc},

    {NULL, NULL}
};

PyDoc_STRVAR(iterationutils_module_name, "_cfuncs");
PyDoc_STRVAR(iterationutils_module_doc, "C Functions\n^^^^^^^^^^^^^^^^");

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  iterationutils_definition = {
    PyModuleDef_HEAD_INIT,
    iterationutils_module_name,         /* module name */
    iterationutils_module_doc,          /* module docstring */
    -1,                                 /* API version */
    iterationutils_methods,             /* module methods */

    NULL, NULL, NULL, NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit__cfuncs(void)
  {
    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;

    // Fill in classes! Must be synced with the Python2 version of module init
    // a few lines later.
    PyTypeObject *typelist[] = {
        &functions_complement_type,
        &recipes_accumulate_type,
        &recipes_applyfunc_type,
        &recipes_grouper_type,
        &recipes_intersperse_type,
        &recipes_merge_type,
        &recipes_roundrobin_type,
        &recipes_successive_type,
        &recipes_uniqueever_type,
        &recipes_uniquejust_type,
        NULL
    };

    m = PyModule_Create(&iterationutils_definition);
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

    return m;
  }

#else

  void
  init_cfuncs(void)
  {
    /* Create the module and add the functions */
    int i;
    PyObject *m;
    char *name;

    // Fill in classes! Must be synced with the Python3 version of module init
    // a few lines earlier.
    PyTypeObject *typelist[] = {
        &functions_complement_type,
        &recipes_accumulate_type,
        &recipes_applyfunc_type,
        &recipes_grouper_type,
        &recipes_intersperse_type,
        &recipes_merge_type,
        &recipes_roundrobin_type,
        &recipes_successive_type,
        &recipes_uniqueever_type,
        &recipes_uniquejust_type,
        NULL
    };

    m = Py_InitModule3(iterationutils_module_name,
                       iterationutils_methods,
                       iterationutils_module_doc);
    if (m == NULL)
        return;

    // Add classes to the module but only use the name starting after the first
    // occurence of ".".
    for (i=0 ; typelist[i] != NULL ; i++) {
        if (PyType_Ready(typelist[i]) < 0)
            return;
        name = strchr(typelist[i]->tp_name, '.');
        assert (name != NULL);
        Py_INCREF(typelist[i]);
        PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
    }
  }
#endif

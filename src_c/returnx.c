#include <Python.h>
#include <structmember.h>

/******************************************************************************
 * Functions always returning the same value.
 *
 * returnTrue   : True
 * returnFalse  : False
 * returnNone   : None
 *****************************************************************************/

static PyObject*
returnx_returnTrue(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(returnx_returnTrue_doc,
"return_True(*args, **kwargs)\n\
\n\
Always return ``True``.\n\
\n\
Parameters\n\
----------\n\
args, kwargs \n\
    any number of positional or keyword parameter.\n\
\n\
Returns\n\
-------\n\
true : bool\n\
    Always returns ``True``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda *args, **kwargs: True`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_True\n\
    >>> return_True(1, 2, 3, 4, a=100)\n\
    True\n\
");

static PyObject*
returnx_returnFalse(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_FALSE;
}

PyDoc_STRVAR(returnx_returnFalse_doc,
"return_False(*args, **kwargs)\n\
\n\
Always return ``False``.\n\
\n\
Parameters\n\
----------\n\
args, kwargs \n\
    any number of positional or keyword parameter.\n\
\n\
Returns\n\
-------\n\
false : bool\n\
    Always returns ``False``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda *args, **kwargs: False`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_False\n\
    >>> return_False(1, 2, 3, 4, a=100)\n\
    False\n\
");

static PyObject*
returnx_returnNone(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_NONE;
}

PyDoc_STRVAR(returnx_returnNone_doc,
"return_None(*args, **kwargs)\n\
\n\
Always return ``None``.\n\
\n\
Parameters\n\
----------\n\
args, kwargs \n\
    any number of positional or keyword parameter.\n\
\n\
Returns\n\
-------\n\
none : None\n\
    Always returns ``None``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda *args, **kwargs: None`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_None\n\
    >>> return_None(1, 2, 3, 4, a=100) is None\n\
    True\n\
");

static PyObject*
returnx_returnFirstPositionalArgument(PyObject *self, PyObject *args, PyObject *keywds) {
    PyObject *first;
    Py_ssize_t tuple_size = PyTuple_Size(args);

    if (tuple_size == 0) {
        PyErr_Format(PyExc_TypeError, "Expected at least one positional argument.");
        return NULL;
    }

    first = PyTuple_GetItem(args, 0);
    if (first == NULL)
        return NULL;

    Py_INCREF(first);
    return first;
}

PyDoc_STRVAR(returnx_returnFirstPositionalArgument_doc,
"return_first_positional_argument(*args, **kwargs)\n\
\n\
Always return the first positional argument given to the function.\n\
\n\
Parameters\n\
----------\n\
args, kwargs \n\
    any number of positional or keyword parameter.\n\
\n\
Returns\n\
-------\n\
first_positional_argument : any type\n\
    Always returns the first positional argument given to the function.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda *args, **kwargs: args[0]`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_first_positional_argument\n\
    >>> return_first_positional_argument(1, 2, 3, 4, a=100)\n\
    1\n\
");


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
returnx_methods[] = {
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

    {"return_first_positional_argument",
     (PyCFunction)returnx_returnFirstPositionalArgument,
     METH_VARARGS | METH_KEYWORDS,
     returnx_returnFirstPositionalArgument_doc},

    {NULL, NULL}
};

PyDoc_STRVAR(returnx_module_name, "_returnx");
PyDoc_STRVAR(returnx_module_doc, "Return Functions\n^^^^^^^^^^^^^^^^");

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  returnx_definition = {
    PyModuleDef_HEAD_INIT,
    returnx_module_name,                           /* module name */
    returnx_module_doc, /* module docstring */
    -1,                                   /* API version */
    returnx_methods,                      /* module methods */

    NULL, NULL, NULL, NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit__returnx(void)
  {
    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;

    // Fill in classes! Must be synced with the Python2 version of module init
    // a few lines later.
    PyTypeObject *typelist[] = {
        //&cls_type,
        NULL
    };

    m = PyModule_Create(&returnx_definition);
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
  init_returnx(void)
  {
    /* Create the module and add the functions */
    int i;
    PyObject *m;
    char *name;

    // Fill in classes! Must be synced with the Python3 version of module init
    // a few lines earlier.
    PyTypeObject *typelist[] = {
        //&cls_type,
        NULL
    };

    m = Py_InitModule3(returnx_module_name, returnx_methods, returnx_module_doc);
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

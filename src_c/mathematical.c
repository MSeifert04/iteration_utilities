#include <Python.h>
#include <structmember.h>

/******************************************************************************
 * Wrapper functions for simple arithmetic functions.
 *
 * square   : identical to pow(x, 2)
 *****************************************************************************/

// global variable
static PyObject *mathematical_long_2 = NULL;

// return global variable or if it's NULL initialize it.
// TODO: That's really weird to do it like this, no idea what the alternative
//       would be...
static PyObject*
mathematical_get_2(void) {
    if (mathematical_long_2 == NULL)
        mathematical_long_2 = PyLong_FromLong((long)2);

    return mathematical_long_2;
}

static PyObject*
mathematical_square(PyObject *self, PyObject *args) {
    return PyNumber_Power(args, mathematical_get_2(), Py_None);
}

PyDoc_STRVAR(mathematical_square_doc,
"square(value) -> squared_value\n\
\n\
Returns the squared `value`.\n\
\n\
Parameters\n\
----------\n\
value : any type\n\
    The value to be squared. The type of the `value` must support ``pow``.\n\
\n\
Returns\n\
-------\n\
square : any type\n\
    Returns ``value**2``.\n\
\n\
Examples\n\
--------\n\
It is not possible to apply ``functools.partial`` to ``pow`` so that one has\n\
a one-argument square function and is significantly faster than ``lambda x: x**2``::\n\
\n\
    >>> from iteration_utilities import square\n\
    >>> square(1)\n\
    1\n\
    >>> square(2.0)\n\
    4.0\n\
");


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
mathematical_methods[] = {

    {"square",
     (PyCFunction)mathematical_square,
     METH_O,
     mathematical_square_doc},

    {NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  mathematical_definition = {
    PyModuleDef_HEAD_INIT,
    "_mathematical",                          /* module name */
    "", /* module docstring */
    -1,                                         /* no idea */
    mathematical_methods,                     /* module methods */

    NULL, NULL, NULL, NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit__mathematical(void)
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

    m = PyModule_Create(&mathematical_definition);
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
  init_mathematical(void)
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

    m = Py_InitModule("_mathematical",
                      mathematical_methods);
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

#include <Python.h>
#include <structmember.h>

/******************************************************************************
 * Functions comparing to a specific value.
 *
 * isNone   : compare to None
 *****************************************************************************/

static PyObject*
isx_IsNone(PyObject *self, PyObject *args) {
    if (args == Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyDoc_STRVAR(isx_IsNone_doc,
"is_None(value) -> bool\n\
\n\
Returns ``True`` if `value` is ``None``, otherwise ``False``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test for ``None``.\n\
\n\
Returns\n\
-------\n\
is_none : bool\n\
    ``True`` if `value` is ``None`` otherwise it returns ``False``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: x is None`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import is_None\n\
    >>> is_None(None)\n\
    True\n\
    >>> is_None(False)\n\
    False\n\
\n\
This can be used for example to remove all ``None`` from an iterable::\n\
\n\
    >>> import sys\n\
    >>> import itertools\n\
    >>> filterfalse = itertools.ifilterfalse if sys.version_info.major == 2 else itertools.filterfalse\n\
    >>> list(filterfalse(is_None, [1, None, 3, 4, 5, None, 7]))\n\
    [1, 3, 4, 5, 7]\n\
");


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
isx_methods[] = {

    {"is_None",
     (PyCFunction)isx_IsNone,
     METH_O,
     isx_IsNone_doc},

    {NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  isx_definition = {
    PyModuleDef_HEAD_INIT,
    "_isx",                         /* module name */
    "",                             /* module docstring */
    -1,                             /* no idea */
    isx_methods,                    /* module methods */

    NULL, NULL, NULL, NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit__isx(void)
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

    m = PyModule_Create(&isx_definition);
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
  init_isx(void)
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

    m = Py_InitModule("_isx",
                      isx_methods);
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

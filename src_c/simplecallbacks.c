#include <Python.h>
#include <structmember.h>

/* Functions always returning the same value.
 *
 * returnTrue   : True
 * returnFalse  : False
 * returnNone   : None
 */
static PyObject*
simplecallbacks_returnTrue(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_TRUE;
}

static PyObject*
simplecallbacks_returnFalse(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_FALSE;
}

static PyObject*
simplecallbacks_returnNone(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_NONE;
}

/* Functions comparing to a specific value.
 *
 * isNone   : compare to None
 */

static PyObject*
simplecallbacks_IsNone(PyObject *self, PyObject *args) {
    if (args == Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

/* Wrapper functions for simple arithmetic functions.
 *
 * square   : identical to pow(x, 2)
 */

static PyObject *simplecallback_long_2 = NULL;

static PyObject*
simplecallback_get_2(void) {
    if (simplecallback_long_2 == NULL)
        simplecallback_long_2 = PyLong_FromLong((long)2);

    return simplecallback_long_2;
}

static PyObject*
simplecallbacks_square(PyObject *self, PyObject *args) {
    return PyNumber_Power(args, simplecallback_get_2(), Py_None);
}


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
simplecallback_methods[] = {
    {"returnTrue", (PyCFunction)simplecallbacks_returnTrue, METH_VARARGS | METH_KEYWORDS,
     "returnTrue(*args, **kwargs) -- Return ``True``."
    },
    {"returnFalse", (PyCFunction)simplecallbacks_returnFalse, METH_VARARGS | METH_KEYWORDS,
     "returnFalse(*args, **kwargs) -- Return ``False``."
    },
    {"returnNone", (PyCFunction)simplecallbacks_returnNone, METH_VARARGS | METH_KEYWORDS,
     "returnNone(*args, **kwargs) -- Return ``None``."
    },
    {"square", (PyCFunction)simplecallbacks_square, METH_O,
     "square(value) -- Return the value squared."
    },
    {"isNone", (PyCFunction)simplecallbacks_IsNone, METH_O,
     "isNone(value) -- Return if the value is ``None``."
    },
    {NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  simplecallback_definition = {
    PyModuleDef_HEAD_INIT,
    "simplecallbacks",
    "\nSimple Callbacks\n^^^^^^^^^^^^^^^^^^\n",
    -1,
    simplecallback_methods,
    NULL,
    NULL,
    NULL,
    NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit_simplecallbacks(void)
  {
    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;
    PyTypeObject *typelist[] = {
        //&filterfalse2_type,
        NULL
    };

    m = PyModule_Create(&simplecallback_definition);
    if (m == NULL)
        return NULL;

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
  initsimplecallbacks(void)
  {
    /* Create the module and add the functions */
    int i;
    PyObject *m;
    char *name;
    PyTypeObject *typelist[] = {
        //&filterfalse_type,
        NULL
    };
    m = Py_InitModule("simplecallbacks", simplecallback_methods);
    if (m == NULL)
        return;

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

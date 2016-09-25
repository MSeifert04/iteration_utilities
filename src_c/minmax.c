#include <Python.h>
#include <structmember.h>


/******************************************************************************
 * Reduce operations
 *
 * minmax   : like min() and max() but in one pass
 *****************************************************************************/

static PyObject *
minmax(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *keyfunc = NULL;
    PyObject *item1 = NULL, *item2 = NULL, *val1 = NULL, *val2 = NULL;
    PyObject *maxitem = NULL, *maxval = NULL, *minitem = NULL, *minval = NULL;
    PyObject *temp = NULL, *emptytuple = NULL, *resulttuple = NULL;

    static char *kwlist[] = {"key", "default", NULL};
    const int positional = PyTuple_Size(args) > 1;
    int ret;
    int cmp;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, "minmax", 1, 1, &sequence)) {
        return NULL;
    }

    emptytuple = PyTuple_New(0);
    if (emptytuple == NULL) {
        return NULL;
    }

    ret = PyArg_ParseTupleAndKeywords(emptytuple, kwds, "|$OO", kwlist,
                                      &keyfunc, &defaultitem);

    Py_DECREF(emptytuple);

    if (!ret) {
        return NULL;
    }

    if (positional && defaultitem != NULL) {
        PyErr_Format(PyExc_TypeError,
                     "Cannot specify a default for minmax with multiple "
                     "positional arguments");
        return NULL;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        return NULL;
    }

    // Iterate over the sequence
    while (( item1 = PyIter_Next(iterator) )) {

        // It could be NULL (end of sequence) but don't care .. yet.
        item2 = PyIter_Next(iterator);

        /* get the value from the key function */
        if (keyfunc != NULL) {
            val1 = PyObject_CallFunctionObjArgs(keyfunc, item1, NULL);
            if (val1 == NULL) {
                goto Fail;
            }
            if (item2 != NULL) {
                val2 = PyObject_CallFunctionObjArgs(keyfunc, item2, NULL);
                if (val2 == NULL) {
                    goto Fail;
                }
            }
        }
        /* no key function; the value is the item */
        else {
            val1 = item1;
            Py_INCREF(val1);
            if (item2 != NULL) {
                val2 = item2;
                Py_INCREF(val2);
            }
        }

        /* maximum value and item are unset; set them */
        if (minval == NULL) {
            if (item2 != NULL) {
                // If both 1 and 2 are set do one compare and set min and max
                // accordingly
                cmp = PyObject_RichCompareBool(val1, val2, Py_LT);
                if (cmp < 0) {
                    goto Fail;
                } else if (cmp > 0) {
                    minval = val1;
                    minitem = item1;
                    maxval = val2;
                    maxitem = item2;
                } else {
                    minval = val2;
                    minitem = item2;
                    maxval = val1;
                    maxitem = item1;
                }
            } else {
                // If only one is set we can set min and max to the only item.
                minitem = item1;
                minval = val1;
                maxitem = item1;
                maxval = val1;
            }
        } else {
            // If the sequence ends and only one element remains we can just
            // set item2/val2 to the last element and skip comparing these two.
            // This "should" keep the ordering, because it's unlikely someone
            // creates a type that is smallest and highest and uses minmax.
            if (item2 == NULL) {
                item2 = item1;
                Py_INCREF(item1);
                val2 = val1;
                Py_INCREF(val1);
            } else {
                // If both are set swap them if val2 is smaller than val1
                cmp = PyObject_RichCompareBool(val2, val1, Py_LT);
                if (cmp < 0) {
                    goto Fail;
                } else if (cmp > 0) {
                    temp = val1;
                    val1 = val2;
                    val2 = temp;

                    temp = item1;
                    item1 = item2;
                    item2 = temp;
                }
            }

            // val1 is smaller or equal to val2 so we compare only val1 with
            // the current minimum
            cmp = PyObject_RichCompareBool(val1, minval, Py_LT);
            if (cmp < 0) {
                goto Fail;
            } else if (cmp > 0) {
                Py_DECREF(minval);
                minval = val1;
                Py_DECREF(minitem);
                minitem = item1;
            } else {
                Py_DECREF(item1);
                Py_DECREF(val1);
            }

            // Same for maximum.
            cmp = PyObject_RichCompareBool(val2, maxval, Py_GT);
            if (cmp < 0) {
                goto Fail;
            } else if (cmp > 0) {
                Py_DECREF(maxval);
                maxval = val2;
                Py_DECREF(maxitem);
                maxitem = item2;
            } else {
                Py_DECREF(item2);
                Py_DECREF(val2);
            }
        }
    }

    if (PyErr_Occurred()) {
        goto Fail;
    }

    if (minval == NULL) {
        assert(maxval == NULL);
        assert(minitem == NULL);
        assert(maxitem == NULL);
        if (defaultitem != NULL) {
            minitem = defaultitem;
            maxitem = defaultitem;
            Py_INCREF(defaultitem);
            Py_INCREF(defaultitem);
        } else {
            PyErr_Format(PyExc_ValueError, "minmax arg is an empty sequence");
            return NULL;
        }
    } else {
        Py_DECREF(minval);
        Py_DECREF(maxval);
    }

    Py_DECREF(iterator);

    resulttuple = PyTuple_Pack(2, minitem, maxitem);
    if (resulttuple == NULL) {
        return NULL;
    }

    Py_DECREF(minitem);
    Py_DECREF(maxitem);

    return resulttuple;

Fail:
    Py_XDECREF(item1);
    Py_XDECREF(item2);
    Py_XDECREF(val1);
    Py_XDECREF(val2);
    Py_XDECREF(minval);
    Py_XDECREF(minitem);
    Py_XDECREF(maxval);
    Py_XDECREF(maxitem);
    Py_DECREF(iterator);
    return NULL;
}


//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
minmax_methods[] = {

    {"c_minmax",
     (PyCFunction)minmax,
     METH_VARARGS | METH_KEYWORDS,
     "minmax"},

    {NULL, NULL}
};

PyDoc_STRVAR(minmax_module_name, "_minmax");
PyDoc_STRVAR(minmax_module_doc, "minmax\n^^^^^^");

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  minmax_definition = {
    PyModuleDef_HEAD_INIT,
    minmax_module_name,                          /* module name */
    minmax_module_doc, /* module docstring */
    -1,                                         /* API version */
    minmax_methods,                     /* module methods */

    NULL, NULL, NULL, NULL
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit__minmax(void)
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

    m = PyModule_Create(&minmax_definition);
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
  init_minmax(void)
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

    m = Py_InitModule3(minmax_module_name, minmax_methods, minmax_module_doc);
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

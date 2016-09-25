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
simplecallbacks_returnTrue(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(simplecallbacks_returnTrue_doc,
"return_True(*args, **kwargs) -> True\n\
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
simplecallbacks_returnFalse(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_FALSE;
}

PyDoc_STRVAR(simplecallbacks_returnFalse_doc,
"return_False(*args, **kwargs) -> False\n\
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
simplecallbacks_returnNone(PyObject *self, PyObject *args, PyObject *keywds) {
    Py_RETURN_NONE;
}

PyDoc_STRVAR(simplecallbacks_returnNone_doc,
"return_None(*args, **kwargs) -> None\n\
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
simplecallbacks_returnFirstPositionalArgument(PyObject *self, PyObject *args, PyObject *keywds) {
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

PyDoc_STRVAR(simplecallbacks_returnFirstPositionalArgument_doc,
"return_first_positional_argument(*args, **kwargs) -> args[0]\n\
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

/******************************************************************************
 * Functions comparing to a specific value.
 *
 * isNone   : compare to None
 *****************************************************************************/

static PyObject*
simplecallbacks_IsNone(PyObject *self, PyObject *args) {
    if (args == Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyDoc_STRVAR(simplecallbacks_IsNone_doc,
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

/******************************************************************************
 * Wrapper functions for simple arithmetic functions.
 *
 * square   : identical to pow(x, 2)
 *****************************************************************************/

// global variable
static PyObject *simplecallback_long_2 = NULL;

// return global variable or if it's NULL initialize it.
// TODO: That's really weird to do it like this, no idea what the alternative
//       would be...
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

PyDoc_STRVAR(simplecallbacks_square_doc,
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


/******************************************************************************
 * Reduce operations
 *
 * minmax   : like min() and max() but in one pass
 *****************************************************************************/

static PyObject *
simplecallbacks_minmax(PyObject *self, PyObject *args, PyObject *kwds)
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
simplecallback_methods[] = {
    {"return_True",
     (PyCFunction)simplecallbacks_returnTrue,
     METH_VARARGS | METH_KEYWORDS,
     simplecallbacks_returnTrue_doc},

    {"return_False",
     (PyCFunction)simplecallbacks_returnFalse,
     METH_VARARGS | METH_KEYWORDS,
     simplecallbacks_returnFalse_doc},

    {"return_None",
     (PyCFunction)simplecallbacks_returnNone,
     METH_VARARGS | METH_KEYWORDS,
     simplecallbacks_returnNone_doc},

    {"return_first_positional_argument",
     (PyCFunction)simplecallbacks_returnFirstPositionalArgument,
     METH_VARARGS | METH_KEYWORDS,
     simplecallbacks_returnFirstPositionalArgument_doc},

    {"square",
     (PyCFunction)simplecallbacks_square,
     METH_O,
     simplecallbacks_square_doc},

    {"is_None",
     (PyCFunction)simplecallbacks_IsNone,
     METH_O,
     simplecallbacks_IsNone_doc},

    {"c_minmax",
     (PyCFunction)simplecallbacks_minmax,
     METH_VARARGS | METH_KEYWORDS,
     "minmax"},

    {NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  simplecallback_definition = {
    PyModuleDef_HEAD_INIT,
    "simplecallbacks",                          /* module name */
    "\nSimple Callbacks\n^^^^^^^^^^^^^^^^^^\n", /* module docstring */
    -1,                                         /* no idea */
    simplecallback_methods,                     /* module methods */

    NULL, NULL, NULL, NULL
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

    // Fill in classes! Must be synced with the Python2 version of module init
    // a few lines later.
    PyTypeObject *typelist[] = {
        //&cls_type,
        NULL
    };

    m = PyModule_Create(&simplecallback_definition);
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
  initsimplecallbacks(void)
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

    m = Py_InitModule("simplecallbacks",
                      simplecallback_methods);
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

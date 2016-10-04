static PyObject *
reduce_nth(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *iterable, *defaultitem=NULL, *func=NULL;
    Py_ssize_t n;
    int truthy=1, retpred=0;

    PyObject *iterator, *item=NULL, *last=NULL, *val=NULL;
    Py_ssize_t i;
    int ok;

    static char *kwlist[] = {"iterable", "n", "default", "pred", "truthy",
                             "retpred", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "On|OOii:nth", kwlist,
                                     &iterable, &n, &defaultitem, &func,
                                     &truthy, &retpred)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    for (i=0 ; i<=n ; ) {
        item = (*Py_TYPE(iterator)->tp_iternext)(iterator);
        if (item == NULL) {
            Py_XDECREF(last);
            last = NULL;
            break;
        }
        // Sequence contains an element and func is None: return it.
        if (func == NULL) {
            if (last != NULL) {
                Py_DECREF(last);
            }
            last = item;
            i++;
            continue;

        } else if (func == Py_None || func == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);

        } else {
            val = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(item);
                Py_XDECREF(last);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
        }

        if (ok == truthy) {
            if (retpred) {
                Py_DECREF(item);
                if (val == NULL) {
                    val = PyBool_FromLong(ok);
                }
                if (last != NULL) {
                    Py_DECREF(last);
                }
                last = val;
                i++;

            } else {
                Py_XDECREF(val);
                if (last != NULL) {
                    Py_DECREF(last);
                }
                last = item;
                i++;
            }

        } else if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            Py_XDECREF(val);
            return NULL;

        } else {
            Py_DECREF(item);
            Py_XDECREF(val);
        }
    }

    Py_DECREF(iterator);

    PyErr_Clear();

    if (last != NULL) {
        return last;
    }

    Py_XDECREF(last);

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_IndexError, "not enough values.");
        return NULL;
    // Otherwise return the default item
    } else {
        // Does it need to be incref'd?
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


PyDoc_STRVAR(reduce_nth_doc, "nth(iterable, n[, default])\n\
\n\
Returns the `n`-th item or a `default` value.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` from which to take the item.\n\
\n\
n : :py:class:`int`\n\
    Index of the item.\n\
\n\
default : any type, optional\n\
    `Default` value if the iterable doesn't contain the index.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
nth_item : any type\n\
    The `n`-th item of the `iterable` or `default` if the index wasn't\n\
    present in the `iterable`.\n\
\n\
Raises\n\
------\n\
IndexError\n\
    If the `iterable` contains less than `n` items and `default` is not given.\n\
\n\
Examples\n\
--------\n\
Without `default` value::\n\
\n\
    >>> from iteration_utilities import nth\n\
    >>> g = (x**2 for x in range(10))\n\
    >>> nth(g, 5)\n\
    25\n\
\n\
Or with `default` if the index is not present::\n\
\n\
    >>> g = (x**2 for x in range(10))\n\
    >>> nth(g, 15, 0)\n\
    0\n\
");

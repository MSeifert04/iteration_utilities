static PyObject *
reduce_nth(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *iterable, *defaultitem=NULL, *func=NULL;
    PyObject *(*iternext)(PyObject *);
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

    iternext = *Py_TYPE(iterator)->tp_iternext;

    for (i=0 ; i<=n || n < 0; ) {
        item = iternext(iterator);
        if (item == NULL) {
            // Keep the last one in case we looked for the last one.
            if (n >= 0) {
                Py_XDECREF(last);
                last = NULL;
            }
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

            } else {
                Py_XDECREF(val);
                if (last != NULL) {
                    Py_DECREF(last);
                }
                last = item;
            }
            i++;

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
Returns the `n`-th value in the `iterable` or `default`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to determine the nth value.\n\
\n\
n : int\n\
    The index of the wanted item. If negative the last item is searched.\n\
\n\
default : any type, optional\n\
    If no nth value is found and `default` is given the `default` is \n\
    returned.\n\
\n\
pred : callable, optional\n\
    If given return the nth item for which ``pred(item)`` is ``True``.\n\
\n\
truthy : bool, optional\n\
    If ``False`` search for the nth item for which ``pred(item)`` is ``False``.\n\
    Default is ``True``.\n\
\n\
    .. note::\n\
       Parameter is ignored if `pred` is not given.\n\
\n\
retpred : bool, optional\n\
    If given return ``pred(item)`` instead of ``item``.\n\
    Default is ``False``.\n\
\n\
    .. note::\n\
       Parameter is ignored if `pred` is not given.\n\
\n\
Returns\n\
-------\n\
nth : any type\n\
    The last value or the nth value for which `pred` is ``True``.\n\
    If there is no such value then `default` is returned.\n\
\n\
Raises\n\
-------\n\
TypeError :\n\
    If there is no nth element and no `default` is given.\n\
\n\
Examples\n\
--------\n\
Some basic examples including the use of ``pred``::\n\
\n\
    >>> from iteration_utilities import nth\n\
    >>> # First item\n\
    >>> nth([0, 1, 2], 0)\n\
    0\n\
    >>> # Second item\n\
    >>> nth([0, 1, 2], 1)\n\
    1\n\
    >>> # Last item\n\
    >>> nth([0, 1, 2], -1)\n\
    2\n\
    \n\
    >>> nth([0, 10, '', tuple(), 20], 1, pred=bool)\n\
    20\n\
    \n\
    >>> # second odd number\n\
    >>> nth([0, 2, 3, 5, 8, 9, 10], 1, pred=lambda x: x%2)\n\
    5\n\
    \n\
    >>> # default value if empty or no true value\n\
    >>> nth([], 0, default=100)\n\
    100\n\
    >>> nth([0, 10, 0, 0], -1, pred=bool, default=100)\n\
    10\n\
\n\
Given a `pred` it is also possible to look for the nth ``False`` value and \n\
return the result of ``pred(item)``::\n\
\n\
    >>> nth([1,2,0], 1, pred=bool)\n\
    2\n\
    >>> nth([1,0,2,0], -1, pred=bool, truthy=False)\n\
    0\n\
    >>> import operator\n\
    >>> nth([[0,3], [0,1], [0,2]], -1, pred=operator.itemgetter(1))\n\
    [0, 2]\n\
    >>> nth([[0,3], [0,1], [0,2]], -1, pred=operator.itemgetter(1), retpred=True)\n\
    2\n\
");

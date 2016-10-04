static PyObject *
reduce_last(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *func = NULL, *last = NULL;
    PyObject *item = NULL, *val = NULL;
    long ok;
    int truthy = 1;
    int retpred = 0;

    static char *kwlist[] = {"iterable", "default", "pred", "truthy", "retpred", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOii:last", kwlist,
            &sequence, &defaultitem, &func, &truthy, &retpred)) {
        return NULL;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        return NULL;
    }

    // Iterate over the sequence
    while (( item = PyIter_Next(iterator) )) {

        // Sequence contains an element and func is None: return it.
        if (func == NULL) {
            if (last != NULL) {
                Py_DECREF(last);
            }
            last = item;
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

    if (PyErr_Occurred()) {
        return NULL;
    }

    if (last != NULL) {
        return last;
    }

    Py_XDECREF(last);

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_TypeError, "no last element found.");
        return NULL;
    // Otherwise return the default item
    } else {
        // Does it need to be incref'd?
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


PyDoc_STRVAR(reduce_last_doc, "last(iterable[, default])\n\
last(iterable[, default, pred, truthy, retpred])\n\
\n\
Returns the last value in the `iterable` or `default`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to determine the last value.\n\
\n\
default : any type, optional\n\
    If no last value is found and `default` is given the `default` is \n\
    returned.\n\
\n\
pred : callable, optional\n\
    If given return the last item for which ``pred(item)`` is ``True``.\n\
\n\
truthy : bool, optional\n\
    If ``False`` search for the last item for which ``pred(item)`` is ``False``.\n\
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
last : any type\n\
    The last value or the last value for which `pred` is ``True``.\n\
    If there is no such value then `default` is returned.\n\
\n\
Raises\n\
-------\n\
TypeError :\n\
    If there is no last element and no `default` is given.\n\
\n\
Examples\n\
--------\n\
Some basic examples including the use of ``pred``::\n\
\n\
    >>> from iteration_utilities import last\n\
    >>> last([0, 1, 2])\n\
    2\n\
    \n\
    >>> last([0, 10, '', tuple(), 20], pred=bool)\n\
    20\n\
    \n\
    >>> # last odd number\n\
    >>> last([0, 2, 3, 5, 8, 9, 10], pred=lambda x: x%2)\n\
    9\n\
    \n\
    >>> # default value if empty or no true value\n\
    >>> last([], default=100)\n\
    100\n\
    >>> last([0, 10, 0, 0], pred=bool, default=100)\n\
    10\n\
\n\
Given a `pred` it is also possible to look for the last ``False`` value and \n\
return the result of ``pred(item)``::\n\
\n\
    >>> last([1,2,0], pred=bool)\n\
    2\n\
    >>> last([1,0,2,0], pred=bool, truthy=False)\n\
    0\n\
    >>> import operator\n\
    >>> last([[0,3], [0,1], [0,2]], pred=operator.itemgetter(1))\n\
    [0, 2]\n\
    >>> last([[0,3], [0,1], [0,2]], pred=operator.itemgetter(1), retpred=True)\n\
    2\n\
");

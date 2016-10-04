static PyObject *
reduce_second(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *func = NULL, *first = NULL;
    PyObject *item = NULL, *val = NULL;
    long ok;
    int truthy = 1;
    int retpred = 0;

    static char *kwlist[] = {"iterable", "default", "pred", "truthy", "retpred", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOii:second", kwlist,
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
            if (first == NULL) {
                first = item;
                continue;
            } else {
                Py_DECREF(iterator);
                Py_DECREF(first);
                return item;
            }

        } else if (func == Py_None || func == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);

        } else {
            val = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(item);
                Py_XDECREF(first);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
        }

        if (ok == truthy) {
            if (first == NULL) {
                Py_XDECREF(val);
                first = item;
                continue;
            }
            Py_DECREF(iterator);
            if (retpred) {
                Py_DECREF(item);
                if (val == NULL) {
                    val = PyBool_FromLong(ok);
                }
                Py_DECREF(first);
                return val;
            } else {
                Py_DECREF(first);
                Py_XDECREF(val);
                return item;
            }

        } else if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            Py_DECREF(val);
            Py_XDECREF(first);
            return NULL;
        }
        Py_DECREF(item);
        Py_XDECREF(val);
    }

    Py_DECREF(iterator);
    Py_XDECREF(first);

    if (PyErr_Occurred()) {
        return NULL;
    }

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_TypeError, "no second element found.");
        return NULL;
    // Otherwise return the default item
    } else {
        // Does it need to be incref'd?
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


PyDoc_STRVAR(reduce_second_doc, "second(iterable[, default])\n\
second(iterable[, default, pred, truthy, retpred])\n\
\n\
Returns the second value in the `iterable` or `default`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to determine the second value.\n\
\n\
default : any type, optional\n\
    If no second value is found and `default` is given the `default` is \n\
    returned.\n\
\n\
pred : callable, optional\n\
    If given return the second item for which ``pred(item)`` is ``True``.\n\
\n\
truthy : bool, optional\n\
    If ``False`` search for the second item for which ``pred(item)`` is ``False``.\n\
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
second : any type\n\
    The second value or the second value for which `pred` is ``True``.\n\
    If there is no such value then `default` is returned.\n\
\n\
Raises\n\
-------\n\
TypeError :\n\
    If there is no second element and no `default` is given.\n\
\n\
Examples\n\
--------\n\
Some basic examples including the use of ``pred``::\n\
\n\
    >>> from iteration_utilities import second\n\
    >>> second([0, 1, 2])\n\
    1\n\
    \n\
    >>> second([0, 10, '', tuple(), 20], pred=bool)\n\
    20\n\
    \n\
    >>> # second odd number\n\
    >>> second([0, 2, 3, 5, 8, 10], pred=lambda x: x%2)\n\
    5\n\
    \n\
    >>> # default value if empty or no true value\n\
    >>> second([], default=100)\n\
    100\n\
    >>> second([0, 10, 0, 0], pred=bool, default=100)\n\
    100\n\
\n\
Given a `pred` it is also possible to look for the second ``False`` value and \n\
return the result of ``pred(item)``::\n\
\n\
    >>> second([1,2,0], pred=bool)\n\
    2\n\
    >>> second([1,0,2,0], pred=bool, truthy=False)\n\
    0\n\
    >>> import operator\n\
    >>> second([[1,0], [0,1], [0,2]], pred=operator.itemgetter(1))\n\
    [0, 2]\n\
    >>> second([[1,0], [0,1], [0,2]], pred=operator.itemgetter(1), retpred=True)\n\
    2\n\
");

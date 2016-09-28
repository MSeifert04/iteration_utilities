static PyObject *
recipes_partition(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "func", NULL};
    PyObject *iterable=NULL, *func=Py_None;
    PyObject *iterator, *item, *result1, *result2, *result, *temp;

    long ok;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O:partition",
                                     kwargs, &iterable, &func)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    result1 = PyList_New(0);
    result2 = PyList_New(0);

    if (result1 == NULL || result2 == NULL) {
        Py_XDECREF(result1);
        Py_DECREF(iterator);
        return NULL;
    }

    if (func == Py_None || func == (PyObject *)&PyBool_Type) {
        func = NULL;
    }

    while ((item = PyIter_Next(iterator))) {

        if (func == NULL) {
            temp = item;
            Py_INCREF(item);
        } else {
            temp = PyObject_CallFunctionObjArgs(func, item, NULL);
        }

        if (temp == NULL) {
            goto Fail;
        }

        ok = PyObject_IsTrue(temp);

        if (ok == 1) {
            if (PyList_Append(result2, item) < 0) {
                goto Fail;
            }
        } else if (ok == 0) {
            if (PyList_Append(result1, item) < 0) {
                goto Fail;
            }
        } else if (ok < 0) {
            goto Fail; // Maybe need to set an exception here...
        }
        Py_DECREF(item);
        Py_DECREF(temp);
    }
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        Py_DECREF(result1);
        Py_DECREF(result2);
        return NULL;
    }

    result = PyTuple_Pack(2, result1, result2);

    Py_DECREF(result1);
    Py_DECREF(result2);

    if (result == NULL) {
        return NULL;
    }

    return result;

Fail:
    Py_XDECREF(result1);
    Py_XDECREF(result2);
    Py_XDECREF(item);
    Py_XDECREF(temp);
    Py_XDECREF(iterator);
    return NULL;
}


PyDoc_STRVAR(recipes_partition_doc, "partition(iterable, func])\n\
\n\
Use a predicate to partition entries into ``False`` entries and ``True``\n\
entries.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` to partition.\n\
\n\
pred : callable or None, optional\n\
    The predicate which determines the partition.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
false_values : list\n\
    An list containing the values for which the predicate was False.\n\
\n\
true_values : list\n\
    An list containing the values for which the predicate was True.\n\
\n\
See also\n\
--------\n\
._core.ipartition : Generator variant of partition.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import partition\n\
>>> def is_odd(val): return val % 2\n\
>>> partition(range(10), is_odd)\n\
([0, 2, 4, 6, 8], [1, 3, 5, 7, 9])\n\
\n\
.. warning::\n\
    In case the `pred` is expensive then ``partition`` can be noticable\n\
    faster than ``ipartition``.");

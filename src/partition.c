/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject *
PyIU_Partition(PyObject *m,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "func", NULL};
    PyObject *iterable=NULL, *func=Py_None;
    PyObject *iterator, *item, *result1, *result2, *result, *temp=NULL;
    PyObject *funcargs=NULL, *tmp=NULL;
    long ok;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:partition", kwlist,
                                     &iterable, &func)) {
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
        Py_XDECREF(result2);
        Py_DECREF(iterator);
        return NULL;
    }

    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        Py_XDECREF(result1);
        Py_XDECREF(result2);
        Py_DECREF(iterator);
        return NULL;
    }

    if (func == Py_None || func == (PyObject *)&PyBool_Type) {
        func = NULL;
    }

    while ((item = (*Py_TYPE(iterator)->tp_iternext)(iterator))) {

        if (func == NULL || func == (PyObject *)&PyBool_Type) {
            temp = item;
            Py_INCREF(item);
        } else {
            PYIU_RECYCLE_ARG_TUPLE(funcargs, item, tmp, goto Fail)
            temp = PyObject_Call(func, funcargs, NULL);
            if (temp == NULL) {
                goto Fail;
            }
        }

        ok = PyObject_IsTrue(temp);
        Py_DECREF(temp);
        temp = NULL;

        if (ok == 1) {
            if (PyList_Append(result2, item) < 0) {
                goto Fail;
            }
        } else if (ok == 0) {
            if (PyList_Append(result1, item) < 0) {
                goto Fail;
            }
        } else {
            goto Fail;
        }
        Py_DECREF(item);
    }

    Py_DECREF(funcargs);
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_DECREF(result1);
            Py_DECREF(result2);
            return NULL;
        }
    }

    result = PyTuple_Pack(2, result1, result2);
    Py_DECREF(result1);
    Py_DECREF(result2);
    if (result == NULL) {
        return NULL;
    }

    return result;

Fail:
    Py_XDECREF(funcargs);
    Py_XDECREF(result1);
    Py_XDECREF(result2);
    Py_XDECREF(item);
    Py_XDECREF(temp);
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(PyIU_Partition_doc, "partition(iterable, func=None)\n\
--\n\
\n\
Use a predicate to partition entries into ``False`` entries and ``True``\n\
entries.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` to partition.\n\
\n\
func : callable or None, optional\n\
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

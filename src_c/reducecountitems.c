/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject * PyIU_Count(PyObject *m, PyObject *args,
                                PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "pred", "eq", NULL};

    PyObject *iterable, *iterator, *item, *val=NULL, *pred=NULL;
    Py_ssize_t sum_int = 0;
    int ok, eq=0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oi:count", kwlist,
                                     &iterable, &pred, &eq)) {
        return NULL;
    }
    if (eq && pred==NULL) {
        PyErr_Format(PyExc_TypeError, "`pred` must be specified if `eq=True`.");
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    while ((item = (*Py_TYPE(iterator)->tp_iternext)(iterator))) {
        // Always check for equality if "eq=1".
        if (eq) {
            ok = PyObject_RichCompareBool(pred, item, Py_EQ);
            Py_DECREF(item);

        // No predicate given just set ok == 1 so the element is counted
        } else if (pred == NULL || pred == Py_None) {
            ok = 1;
            Py_DECREF(item);

        // Predicate is bool, so we can skip the function call and just
        // evaluate if the object is truthy.
        } else if (pred == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);
            Py_DECREF(item);

        // Call the function and check if the returned value is truthy.
        } else {
            val = PyObject_CallFunctionObjArgs(pred, item, NULL);
            Py_DECREF(item);
            if (val == NULL) {
                Py_DECREF(iterator);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
            Py_DECREF(val);
        }

        // If we found a match increment the counter, if we encountered an
        // Exception throw it here.
        if (ok == 1) {
            sum_int++;
        } else if (ok < 0) {
            Py_DECREF(iterator);
            return NULL;
        }

        // check for overflow, no fallback because it's unlikely that we should
        // process some iterable that's longer than the maximum py_ssize_t...
        if (sum_int == PY_SSIZE_T_MAX) {
            Py_DECREF(iterator);
            PyErr_Format(PyExc_TypeError,
                         "`iterable` is too long to count.");
            return NULL;
        }
    }

    PYIU_CLEAR_STOPITERATION;
    Py_DECREF(iterator);

    return PyLong_FromSsize_t(sum_int);
}

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(PyIU_Count_doc, "count_items(iterable[, pred, eq])\n\
\n\
Count how many times the predicate is true.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to count in.\n\
\n\
pred : callable, any type, None, optional\n\
    Predicate to test. Depending on the `eq` parameter this parameter has\n\
    different meanings:\n\
    \n\
    - ``eq=True`` : Each item will be counted if ``item == pred``, the `pred`\n\
      must not be omitted in this case.\n\
    - ``eq=False`` : If ``pred`` is not given or ``None`` then each item in\n\
      the iterable is counted.\n\
      If ``pred`` is given and not ``None`` then each item satisifying\n\
      ``if pred(item)`` is counted.\n\
\n\
eq : bool, optional\n\
    If ``True`` compare each item in the `iterable` to `pred` instead of\n\
    calling ``pred(item)``.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
number : number\n\
    The numer of times the predicate is ``True``.\n\
\n\
Examples\n\
--------\n\
To count how many elements are within an `iterable`::\n\
\n\
    >>> from iteration_utilities import count_items\n\
    >>> count_items([0, 0, '', {}, [], 2])\n\
    6\n\
\n\
To count the number of truthy values::\n\
\n\
    >>> count_items([0, 0, '', {}, [], 2], pred=bool)\n\
    1\n\
\n\
To count the number of values satisfying a condition::\n\
\n\
    >>> def smaller5(val): return val < 5\n\
    >>> count_items([1, 2, 3, 4, 5, 6, 6, 7], smaller5)\n\
    4\n\
\n\
To count the number of values equal to another value::\n\
\n\
    >>> count_items([1, 2, 3, 4, 5, 6, 6, 7], 6, True)\n\
    2");

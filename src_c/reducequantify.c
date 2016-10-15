static PyObject * PyIU_Quantify(PyObject *m, PyObject *args,
                                PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "pred", NULL};
    PyObject *iterable, *iterator, *item, *val, *pred=NULL;
    Py_ssize_t sum_int = 0;
    int ok;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:quantify", kwlist,
                                     &iterable, &pred)) {
        return NULL;
    }

    /* Get iterator. */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    // Fast version with integer increment
    while ((item = (*Py_TYPE(iterator)->tp_iternext)(iterator))) {
        if (pred == NULL || pred == Py_None) {
            val = item;
        } else {
            val = PyObject_CallFunctionObjArgs(pred, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(item);
                return NULL;
            }
        }

        ok = PyObject_IsTrue(val);
        if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            Py_XDECREF(val);
            return NULL;
        } else if (ok == 1) {
            sum_int++;
        }

        if (sum_int == PY_SSIZE_T_MAX) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            Py_XDECREF(val);
            PyErr_Format(PyExc_TypeError,
                         "`iterable` is too long to compute the sum.");
            return NULL;
        }
        Py_DECREF(item);
        Py_XDECREF(val);
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

PyDoc_STRVAR(PyIU_Quantify_doc, "quantify(iterable, pred)\n\
\n\
Count how many times the predicate is true.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to count in.\n\
\n\
pred : callable, optional\n\
    Predicate to test.\n\
    Default is :py:class:`bool`.\n\
\n\
Returns\n\
-------\n\
number : number\n\
    The numer of times the predicate is ``True``.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import quantify\n\
>>> quantify([0,0,'',{}, [], 2])\n\
1\n\
\n\
>>> def smaller5(val): return val < 5\n\
>>> quantify([1,2,3,4,5,6,6,7], smaller5)\n\
4\n\
");

/******************************************************************************
 * Functions comparing to a specific value.
 *****************************************************************************/

static PyObject*
isx_IsNone(PyObject *self, PyObject *args) {
    if (args == Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}


static PyObject*
isx_IsNotNone(PyObject *self, PyObject *args) {
    if (args != Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyDoc_STRVAR(isx_IsNone_doc,
"is_None(value)\n\
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

PyDoc_STRVAR(isx_IsNotNone_doc,
"is_not_None(value)\n\
\n\
Returns ``False`` if `value` is ``None``, otherwise ``True``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test for ``None``.\n\
\n\
Returns\n\
-------\n\
is_not_none : bool\n\
    ``False`` if `value` is ``None`` otherwise it returns ``True``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: x is not None`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import is_not_None\n\
    >>> is_not_None(None)\n\
    False\n\
    >>> is_not_None(False)\n\
    True\n\
\n\
");


static PyObject *isx_long_2 = NULL;


static PyObject* isx_get_2(void) {
    if (isx_long_2 == NULL)
        isx_long_2 = PyLong_FromLong((long)2);

    return isx_long_2;
}

/******************************************************************************
 * Even/Odd
 *****************************************************************************/

static PyObject*
isx_IsEven(PyObject *self, PyObject *args) {
    PyObject *val = PyNumber_Remainder(args, isx_get_2());
    int res = PyObject_IsTrue(val);
    Py_DECREF(val);
    if (res > 0) {
        Py_RETURN_FALSE;
    } else if (res == 0) {
        Py_RETURN_TRUE;
    } else {
        return NULL;
    }
}

static PyObject*
isx_IsOdd(PyObject *self, PyObject *args) {
    PyObject *val = PyNumber_Remainder(args, isx_get_2());
    int res = PyObject_IsTrue(val);
    Py_DECREF(val);
    if (res > 0) {
        Py_RETURN_TRUE;
    } else if (res == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

PyDoc_STRVAR(isx_IsEven_doc,
"is_even(value)\n\
\n\
Returns ``True`` if `value` is even, otherwise ``False``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test if even.\n\
\n\
Returns\n\
-------\n\
is_even : bool\n\
    ``True`` if `value` is even otherwise it returns ``False``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: not x % 2`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import is_even\n\
    >>> is_even(0)\n\
    True\n\
    >>> is_even(1)\n\
    False\n\
    >>> is_even(2)\n\
    True\n\
\n\
");


PyDoc_STRVAR(isx_IsOdd_doc,
"is_odd(value)\n\
\n\
Returns ``True`` if `value` is odd, otherwise ``False``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test if odd.\n\
\n\
Returns\n\
-------\n\
is_odd : bool\n\
    ``True`` if `value` is odd otherwise it returns ``False``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: bool(x % 2)`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import is_odd\n\
    >>> is_odd(0)\n\
    False\n\
    >>> is_odd(1)\n\
    True\n\
    >>> is_odd(2)\n\
    False\n\
\n\
");


/******************************************************************************
 * Iterable
 *****************************************************************************/

static PyObject*
isx_IsIterable(PyObject *self, PyObject *o) {
    PyTypeObject *t = o->ob_type;
    getiterfunc f = NULL;
    f = t->tp_iter;

    if (f == NULL) {
        if (PySequence_Check(o)) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }

    else {
        PyObject *res = (*f)(o);
        if (res != NULL && !PyIter_Check(res)) {
            Py_DECREF(res);
            Py_RETURN_FALSE;
        } else {
            Py_DECREF(res);
            Py_RETURN_TRUE;
        }
    }
}


PyDoc_STRVAR(isx_IsIterable_doc,
"is_iterable(value)\n\
\n\
Returns ``True`` if `value` is iterable, otherwise ``False``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test if iterable.\n\
\n\
Returns\n\
-------\n\
is_iterable : bool\n\
    ``True`` if `value` is iterable otherwise it returns ``False``.\n\
\n\
Examples\n\
--------\n\
A few simple examples::\n\
\n\
    >>> from iteration_utilities import is_iterable\n\
    >>> is_iterable(0)\n\
    False\n\
    >>> is_iterable('abc')\n\
    True\n\
    >>> is_iterable([1,2,3])\n\
    True\n\
\n\
");

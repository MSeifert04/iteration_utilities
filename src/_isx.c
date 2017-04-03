/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 * is_None
 *
 * equivalent to:
 *
 * lambda value: value is None
 *****************************************************************************/

static PyObject *
PyIU_IsNone(PyObject *m,
            PyObject *o)
{
    if (o == Py_None) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/******************************************************************************
 * is_not_None
 *
 * equivalent to:
 *
 * lambda value: value is not None
 *****************************************************************************/

static PyObject *
PyIU_IsNotNone(PyObject *m,
               PyObject *o)
{
    if (o != Py_None) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/******************************************************************************
 * is_even
 *
 * equivalent to:
 *
 * lambda value: value % 2 == 0
 *****************************************************************************/

static PyObject *
PyIU_IsEven(PyObject *m,
            PyObject *o)
{
    PyObject *remainder;
    int res;

    remainder = PyNumber_Remainder(o, PyIU_global_two);
    if (remainder == NULL) {
        return NULL;
    }

    res = PyObject_IsTrue(remainder);
    Py_DECREF(remainder);

    if (res > 0) {
        Py_RETURN_FALSE;
    } else if (res == 0) {
        Py_RETURN_TRUE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * is_odd
 *
 * equivalent to:
 *
 * lambda value: value % 2 != 0
 *****************************************************************************/

static PyObject *
PyIU_IsOdd(PyObject *m,
           PyObject *o)
{
    PyObject *remainder;
    int res;

    remainder = PyNumber_Remainder(o, PyIU_global_two);
    if (remainder == NULL) {
        return NULL;
    }

    res = PyObject_IsTrue(remainder);
    Py_DECREF(remainder);

    if (res > 0) {
        Py_RETURN_TRUE;
    } else if (res == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * is_iterable
 *
 * equivalent to:
 *
 * try:
 *     iter(value)
 * except TypeError:
 *     return False
 * else:
 *     return True
 *****************************************************************************/

static PyObject *
PyIU_IsIterable(PyObject *m,
                PyObject *o)
{
    PyObject *it = PyObject_GetIter(o);
    if (it == NULL) {
        if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)) {
            PyErr_Clear();
            Py_RETURN_FALSE;
        } else {
            return NULL;
        }
    } else {
        Py_DECREF(it);
        Py_RETURN_TRUE;
    }
}

/******************************************************************************
 * Documentation
 *****************************************************************************/

PyDoc_STRVAR(PyIU_IsNone_doc, "is_None(value, /)\n\
--\n\
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
is_none : :py:class:`bool`\n\
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
    [1, 3, 4, 5, 7]");

PyDoc_STRVAR(PyIU_IsNotNone_doc, "is_not_None(value, /)\n\
--\n\
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
is_not_none : :py:class:`bool`\n\
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
    True");

PyDoc_STRVAR(PyIU_IsEven_doc, "is_even(value, /)\n\
--\n\
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
is_even : :py:class:`bool`\n\
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
    True");

PyDoc_STRVAR(PyIU_IsOdd_doc, "is_odd(value, /)\n\
--\n\
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
is_odd : :py:class:`bool`\n\
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
    False");

PyDoc_STRVAR(PyIU_IsIterable_doc, "is_iterable(value, /)\n\
--\n\
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
is_iterable : :py:class:`bool`\n\
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
    True");

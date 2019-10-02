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
PyIU_IsNone(PyObject *Py_UNUSED(m),
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
PyIU_IsNotNone(PyObject *Py_UNUSED(m),
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
PyIU_IsEven(PyObject *Py_UNUSED(m),
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
PyIU_IsOdd(PyObject *Py_UNUSED(m),
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
PyIU_IsIterable(PyObject *Py_UNUSED(m),
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

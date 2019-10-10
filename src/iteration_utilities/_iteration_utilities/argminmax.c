/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
argminmax(PyObject *args,
          PyObject *kwargs,
          int cmpop)
{
    static char *kwlist[] = {"key", "default", NULL};

    PyObject *sequence, *keyfunc=NULL, *iterator=NULL;
    PyObject *item=NULL, *val=NULL, *maxval=NULL;
    Py_ssize_t defaultitem=0, idx=-1, maxidx=-1;
    int defaultisset = 0;
    const int positional = PyTuple_GET_SIZE(args) > 1;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, cmpop == Py_LT ? "argmin" : "argmax",
                                  1, 1, &sequence)) {
        return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(
            PyIU_global_0tuple, kwargs,
            cmpop == Py_LT ? "|On:argmin" : "|On:argmax",
            kwlist,
            &keyfunc, &defaultitem)) {
        return NULL;
    }

    if (defaultitem != 0 ||
            (kwargs != NULL && PyDict_CheckExact(kwargs) &&
             PyDict_GetItemString(kwargs, "default"))) {
        defaultisset = 1;
    }
    PYIU_NULL_IF_NONE(keyfunc);
    Py_XINCREF(keyfunc);

    if (positional && defaultisset) {
        PyErr_Format(PyExc_TypeError,
                     "Cannot specify a `default` for `%s` with "
                     "multiple positional arguments",
                     cmpop == Py_LT ? "argmin" : "argmax");
        goto Fail;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        goto Fail;
    }

    while ( (item=Py_TYPE(iterator)->tp_iternext(iterator)) ) {
        idx++;

        /* Use the item itself or keyfunc(item). */
        if (keyfunc != NULL) {
            val = PyIU_CallWithOneArgument(keyfunc, item);
            if (val == NULL) {
                goto Fail;
            }
        } else {
            val = item;
            Py_INCREF(val);
        }

        /* maximum value and item are unset; set them. */
        if (maxval == NULL) {
            maxval = val;
            maxidx = idx;

        /* maximum value and item are set; update them as necessary. */
        } else {
            int cmpres = PyObject_RichCompareBool(val, maxval, cmpop);
            if (cmpres > 0) {
                Py_DECREF(maxval);
                maxval = val;
                maxidx = idx;
            } else if (cmpres == 0) {
                Py_DECREF(val);
            } else {
                goto Fail;
            }
        }
        Py_DECREF(item);
    }

    Py_DECREF(iterator);
    Py_XDECREF(maxval);
    Py_XDECREF(keyfunc);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            return NULL;
        }
    }

    if (maxidx == -1) {
        if (defaultisset) {
            maxidx = defaultitem;
        } else {
            PyErr_Format(PyExc_ValueError,
                         "`%s` `iterable` is an empty sequence",
                         cmpop == Py_LT ? "argmin" : "argmax");
            return NULL;
        }
    }
#if PY_MAJOR_VERSION == 2
    return PyInt_FromSsize_t(maxidx);
#else
    return PyLong_FromSsize_t(maxidx);
#endif

Fail:
    Py_XDECREF(keyfunc);
    Py_XDECREF(item);
    Py_XDECREF(val);
    Py_XDECREF(maxval);
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Argmin
 *****************************************************************************/

static PyObject *
PyIU_Argmin(PyObject *Py_UNUSED(m),
            PyObject *args,
            PyObject *kwargs)
{
    return argminmax(args, kwargs, Py_LT);
}

/******************************************************************************
 * Argmax
 *****************************************************************************/

static PyObject *
PyIU_Argmax(PyObject *Py_UNUSED(m),
            PyObject *args,
            PyObject *kwargs)
{
    return argminmax(args, kwargs, Py_GT);
}

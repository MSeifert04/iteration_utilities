/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_MinMax(PyObject *Py_UNUSED(m),
            PyObject *args,
            PyObject *kwargs)
{
    static char *kwlist[] = {"key", "default", NULL};

    PyObject *sequence, *iterator=NULL, *defaultitem = NULL, *keyfunc = NULL;
    PyObject *item1 = NULL, *item2 = NULL, *val1 = NULL, *val2 = NULL;
    PyObject *maxitem = NULL, *maxval = NULL, *minitem = NULL, *minval = NULL;
    PyObject *temp = NULL, *resulttuple = NULL;
    int cmp;
    const int positional = PyTuple_GET_SIZE(args) > 1;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, "minmax", 1, 1, &sequence)) {
        return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(PyIU_global_0tuple, kwargs,
                                     "|OO:minmax", kwlist,
                                     &keyfunc, &defaultitem)) {
        return NULL;
    }

    PYIU_NULL_IF_NONE(keyfunc);
    Py_XINCREF(keyfunc);

    if (positional && defaultitem != NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Cannot specify a default for `minmax` with multiple "
                        "positional arguments");
        goto Fail;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        goto Fail;
    }

    while ( (item1=Py_TYPE(iterator)->tp_iternext(iterator)) ) {
        item2 = Py_TYPE(iterator)->tp_iternext(iterator);
        /* item2 could be NULL (end of sequence) clear a StopIteration but
           immediatly fail if it's another exception. It will check for
           exceptions in the end (again) but make sure it does not process
           an iterable when the iterator threw an exception! */
        if (item2 == NULL) {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }
        }

        /* get the value from the key function. */
        if (keyfunc != NULL) {
            val1 = PyIU_CallWithOneArgument(keyfunc, item1);
            if (val1 == NULL) {
                goto Fail;
            }
            if (item2 != NULL) {
                val2 = PyIU_CallWithOneArgument(keyfunc, item2);
                if (val2 == NULL) {
                    goto Fail;
                }
            }

        /* no key function; the value is the item. */
        } else {
            val1 = item1;
            Py_INCREF(val1);
            if (item2 != NULL) {
                val2 = item2;
                Py_INCREF(val2);
            }
        }

        /* maximum value and item are unset; set them. */
        if (minval == NULL) {
            if (item2 != NULL) {
                /* If both 1 and 2 are set do one compare and set min and max
                   accordingly.
                   */
                cmp = PyObject_RichCompareBool(val1, val2, Py_LT);
                if (cmp < 0) {
                    goto Fail;
                } else if (cmp > 0) {
                    minval = val1;
                    minitem = item1;
                    maxval = val2;
                    maxitem = item2;
                } else {
                    /* To keep stability we need to check if they are equal and
                       only use val2 as minimum IF it's really smaller.
                       */
                    cmp = PyObject_RichCompareBool(val1, val2, Py_GT);
                    if (cmp < 0) {
                        /* Should really be impossible because it already
                           worked with LT but maybe we got some weird class
                           here...
                           */
                        goto Fail;
                    } else if (cmp > 0) {
                        minval = val2;
                        minitem = item2;
                        maxval = val1;
                        maxitem = item1;
                    } else {
                        minval = val1;
                        minitem = item1;
                        maxval = val1;
                        maxitem = item1;
                        Py_INCREF(item1);
                        Py_INCREF(val1);
                        Py_DECREF(item2);
                        Py_DECREF(val2);
                    }
                }
            } else {
                /* If only one is set we can set min and max to the only item.
                   */
                minitem = item1;
                minval = val1;
                maxitem = item1;
                maxval = val1;
                Py_INCREF(item1);
                Py_INCREF(val1);
            }
        } else {
            /* If the sequence ends and only one element remains we can just
               set item2/val2 to the last element and skip comparing these two.
               This "should" keep the ordering, because it's unlikely someone
               creates a type that is smallest and highest and uses minmax.
               */
            if (item2 == NULL) {
                item2 = item1;
                val2 = val1;
                Py_INCREF(item1);
                Py_INCREF(val1);
            } else {
                /* If both are set swap them if val2 is smaller than val1. */
                cmp = PyObject_RichCompareBool(val2, val1, Py_LT);
                if (cmp > 0) {
                    temp = val1;
                    val1 = val2;
                    val2 = temp;

                    temp = item1;
                    item1 = item2;
                    item2 = temp;
                } else if (cmp < 0) {
                    goto Fail;
                }
            }

            /* val1 is smaller or equal to val2 so we compare only val1 with
               the current minimum.
               */
            cmp = PyObject_RichCompareBool(val1, minval, Py_LT);
            if (cmp > 0) {
                Py_DECREF(minval);
                Py_DECREF(minitem);
                minval = val1;
                minitem = item1;
            } else if (cmp == 0) {
                Py_DECREF(item1);
                Py_DECREF(val1);
            } else {
                goto Fail;
            }

            /* Same for maximum. */
            cmp = PyObject_RichCompareBool(val2, maxval, Py_GT);
            if (cmp > 0) {
                Py_DECREF(maxval);
                Py_DECREF(maxitem);
                maxval = val2;
                maxitem = item2;
            } else if (cmp == 0) {
                Py_DECREF(item2);
                Py_DECREF(val2);
            } else  {
                goto Fail;
            }
        }
    }

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            goto Fail;
        }
    }

    if (minval == NULL) {
        if (maxval != NULL || minitem != NULL || maxitem != NULL) {
            /* This should be impossible to reach but better check. */
            goto Fail;
        }
        if (defaultitem != NULL) {
            minitem = defaultitem;
            maxitem = defaultitem;
            Py_INCREF(defaultitem);
            Py_INCREF(defaultitem);
        } else {
            PyErr_SetString(PyExc_ValueError,
                            "`minmax` `iterable` is an empty sequence");
            goto Fail;
        }
    } else {
        Py_DECREF(minval);
        Py_DECREF(maxval);
    }

    Py_DECREF(iterator);
    Py_XDECREF(keyfunc);
    Py_XDECREF(defaultitem);

    resulttuple = PyTuple_Pack(2, minitem, maxitem);
    Py_DECREF(minitem);
    Py_DECREF(maxitem);
    if (resulttuple == NULL) {
        return NULL;
    }

    return resulttuple;

Fail:
    Py_XDECREF(keyfunc);
    Py_XDECREF(defaultitem);
    Py_XDECREF(item1);
    Py_XDECREF(item2);
    Py_XDECREF(val1);
    Py_XDECREF(val2);
    Py_XDECREF(minval);
    Py_XDECREF(minitem);
    Py_XDECREF(maxval);
    Py_XDECREF(maxitem);
    Py_XDECREF(iterator);
    return NULL;
}

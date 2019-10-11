/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_Groupby(PyObject *Py_UNUSED(m),
             PyObject *args,
             PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", "keep", "reduce", "reducestart", NULL};

    PyObject *iterable;
    PyObject *keyfunc;
    PyObject *valfunc = NULL;
    PyObject *iterator = NULL;
    PyObject *reducefunc = NULL;
    PyObject *reducestart = NULL;
    PyObject *resdict = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|OOO:groupedby", kwlist,
                                     &iterable, &keyfunc, &valfunc, &reducefunc,
                                     &reducestart)) {
        goto Fail;
    }
    if (reducefunc == Py_None) {
        reducefunc = NULL;
    }
    if (valfunc == Py_None) {
        valfunc = NULL;
    }

    if (reducefunc == NULL && reducestart != NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot specify `reducestart` argument for "
                        "`groupedby` if no `reduce` is given.");
        goto Fail;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }

    resdict = PyDict_New();
    if (resdict == NULL) {
        goto Fail;
    }

    for (;;) {
        PyObject *item;
        PyObject *val;
        PyObject *keep;

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
        Py_hash_t hash;
#endif

        item = Py_TYPE(iterator)->tp_iternext(iterator);

        if (item == NULL) {
            break;
        }

        /* Calculate the key for the dictionary (val). */
        val = PyIU_CallWithOneArgument(keyfunc, item);
        if (val == NULL) {
            Py_DECREF(item);
            goto Fail;
        }

        /* Calculate the value for the dictionary (keep).  */
        if (valfunc == NULL) {
            keep = item;
        } else {
            /* We use the same item again to calculate the keep so we don't need
               to replace. */
            keep = PyIU_CallWithOneArgument(valfunc, item);
            Py_DECREF(item);
            if (keep == NULL) {
                Py_DECREF(val);
                goto Fail;
            }
        }

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
        /* Taken from dictobject.c CPython 3.5 */
        if (!PyUnicode_CheckExact(val) ||
                (hash = ((PyASCIIObject *) val)->hash) == -1) {
            hash = PyObject_Hash(val);
            if (hash == -1) {
                Py_DECREF(keep);
                Py_DECREF(val);
                goto Fail;
            }
        }
#endif

        /* Keep all values as list.  */
        if (reducefunc == NULL) {
            PyObject *lst;

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
            lst = _PyDict_GetItem_KnownHash(resdict, val, hash);
#else
            lst = PyDict_GetItem(resdict, val);
#endif
            if (lst == NULL) {
                int ok;
                lst = PyList_New(1);
                if (lst == NULL) {
                    Py_DECREF(keep);
                    Py_DECREF(val);
                    goto Fail;
                }
                PyList_SET_ITEM(lst, 0, keep);
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, lst, hash);
#else
                ok = PyDict_SetItem(resdict, val, lst);
#endif
                Py_DECREF(lst);
                Py_DECREF(val);
                if (ok == -1) {
                    goto Fail;
                }
            } else {
                int ok;
                Py_DECREF(val);
                ok = PyList_Append(lst, keep);
                Py_DECREF(keep);
                if (ok < 0) {
                    goto Fail;
                }
            }

        /* Reduce the values with a binary operation. */
        } else {
            PyObject *current;

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
            current = _PyDict_GetItem_KnownHash(resdict, val, hash);
#else
            current = PyDict_GetItem(resdict, val);
#endif
            Py_XINCREF(current);

            /* No item yet and no starting value given: Keep the "keep". */
            if (current == NULL && reducestart == NULL) {
                int ok;
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, keep, hash);
#else
                ok = PyDict_SetItem(resdict, val, keep);
#endif
                Py_DECREF(val);
                Py_DECREF(keep);
                if (ok == -1) {
                    goto Fail;
                }

            /* Already an item present so use the binary operation. */
            } else {
                PyObject *reducetmp;
                int ok;

                if (current == NULL) {
                    reducetmp = PyIU_CallWithTwoArguments(reducefunc, reducestart, keep);
                } else {
                    reducetmp = PyIU_CallWithTwoArguments(reducefunc, current, keep);
                    Py_DECREF(current);
                }
                Py_DECREF(keep);
                if (reducetmp == NULL) {
                    Py_DECREF(val);
                    goto Fail;
                }
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, reducetmp, hash);
#else
                ok = PyDict_SetItem(resdict, val, reducetmp);
#endif
                Py_DECREF(val);
                Py_DECREF(reducetmp);
                if (ok == -1) {
                    goto Fail;
                }
            }
        }
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_DECREF(resdict);
            return NULL;
        }
    }

    return resdict;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(resdict);
    return NULL;
}

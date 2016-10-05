/******************************************************************************
 * Helper function copied from listobject.c (Python module objects)
 *****************************************************************************/
static int
reduce_allequal_helper_list_contains(PyObject *a, PyObject *el)  // PyListObject *a in the original
{
    Py_ssize_t i;
    int cmp;

    for (i = 0, cmp = 0 ; cmp == 0 && i < Py_SIZE(a); ++i)
        cmp = PyObject_RichCompareBool(el, PyList_GET_ITEM(a, i), Py_EQ);
    return cmp;
}


static PyObject *
reduce_alldistinct(PyObject *self, PyObject *iterable)
{
    PyObject *it;
    PyObject *(*iternext)(PyObject *);
    PyObject *item;
    PyObject *seen;
    PyObject *seenlist=NULL;

    int ok;

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        goto Fail;
    }

    seen = PySet_New(NULL);
    if (seen == NULL) {
        goto Fail;
    }

    iternext = *Py_TYPE(it)->tp_iternext;
    for (;;) {
        item = iternext(it);
        if (item == NULL) {
            break;
        }

        ok = PySet_Contains(seen, item);

        if (ok == 0) {
            ok = PySet_Add(seen, item);
            if (ok < 0) {
                goto Fail;
            }

        } else if (ok == 1) {
            Py_XDECREF(it);
            Py_XDECREF(seen);
            Py_XDECREF(seenlist);
            Py_XDECREF(item);
            Py_RETURN_FALSE;

        } else {
            /* Error when checking if the value is in the set.*/
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }

            if (seenlist == NULL) {
                seenlist = PyList_New(0);
                if (seenlist == NULL) {
                    goto Fail;
                }
            }

            ok = reduce_allequal_helper_list_contains(seenlist, item);

            if (ok == 0) {
                ok = PyList_Append(seenlist, item);
                if (ok != 0) {
                    goto Fail;
                }

            } else if (ok == 1) {
                Py_XDECREF(it);
                Py_XDECREF(seen);
                Py_XDECREF(seenlist);
                Py_XDECREF(item);
                Py_RETURN_FALSE;

            } else {
                goto Fail;
            }

        }
    }

    PyErr_Clear();

    Py_XDECREF(it);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);

    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(it);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);
    return NULL;
}


PyDoc_STRVAR(reduce_alldistinct_doc,
"all_distinct(iterable)\n\
");

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
    PyObject *it=NULL;
    PyObject *(*iternext)(PyObject *);
    PyObject *item=NULL;
    PyObject *seen=NULL;
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
    while ( (item = iternext(it)) ) {

        ok = PySet_Contains(seen, item);

        if (ok == 0) {
            ok = PySet_Add(seen, item);
            if (ok < 0) {
                goto Fail;
            }

        } else if (ok == 1) {
            goto Found;

        } else {
            // TypeError when checking if the value is in the set.
            // this means the value is not hashable
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }

            // Create a list for the unhashable values
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
                goto Found;

            } else {
                goto Fail;  // untested code path
            }

        }
        Py_DECREF(item);
    }

    // Prevent to return a pending StopIteration exception from tp_iternext.
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

Found:
    Py_XDECREF(it);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);
    Py_RETURN_FALSE;
}


PyDoc_STRVAR(reduce_alldistinct_doc,
"all_distinct(iterable)\n\
\n\
Checks if all items in the `iterable` are distinct.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
Returns\n\
-------\n\
distinct : bool\n\
    ``True`` if no two values are equal and ``False`` if there is at least\n\
    one duplicate in `iterable`.\n\
\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import all_distinct\n\
>>> all_distinct('AAAABBBCCDAABBB')\n\
False\n\
\n\
>>> all_distinct('abcd')\n\
True\n\
");

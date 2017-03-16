/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 * Missing stdlib MACROS
 *
 * PySet_CheckExact : check for a set but not subtype
 *****************************************************************************/

#define PySet_CheckExact(ob) (Py_TYPE(ob) == &PySet_Type)

/******************************************************************************
 * Compatibility macros
 *
 * Py_RETURN_NOTIMPLEMENTED : sets TypeError and returns NULL.
 *****************************************************************************/

#if PY_MAJOR_VERSION == 2
#define Py_RETURN_NOTIMPLEMENTED \
    return PyErr_Format(PyExc_TypeError, "not implemented."), NULL
#endif

/******************************************************************************
 * Complex Convenience macros
 *
 * TODO: These are much too complex for macros but yield a 20-35% speedup over
 *       functions (which are 10-20% slower than using
 *       PyObject_CallFunctionArgs or similar).
 *
 * PYIU_RECYCLE_ARG_TUPLE :
 *     args (Tuple of length 1)
 *     newarg (PyObject *)
 *     error_stmt (for example "return NULL" or "goto Fail")
 *
 * PYIU_RECYCLE_ARG_TUPLE_BINOP :
 *     args (Tuple of length 1)
 *     new1, new2 (PyObject *)
 *     error_stmt (for example "return NULL" or "goto Fail")
 *****************************************************************************/

#define PYIU_RECYCLE_ARG_TUPLE(args, newarg, error_stmt)                 \
    do {                                                                 \
        if (Py_REFCNT(args) == 1) {                                      \
            /* Recycle args by replacing the element with newarg. */     \
            PyObject *tmp = PyTuple_GET_ITEM(args, 0);                   \
            Py_INCREF(newarg);                                           \
            PyTuple_SET_ITEM(args, 0, newarg);                           \
            Py_XDECREF(tmp);                                             \
        } else {                                                         \
            /* Create a new tuple and insert the newarg. */              \
            PyObject *tmp = args;                                        \
            args = PyTuple_New(1);                                       \
            if (args == NULL) {                                          \
                error_stmt;                                              \
            }                                                            \
            Py_INCREF(newarg);                                           \
            PyTuple_SET_ITEM(args, 0, newarg);                           \
            Py_DECREF(tmp);                                              \
        }                                                                \
    } while (0)


#define PYIU_RECYCLE_ARG_TUPLE_BINOP(args, new1, new2, error_stmt)                \
    do {                                                                          \
        if (Py_REFCNT(args) == 1) {                                               \
            /* Recycle args by replacing the element with newarg. */              \
            PyObject *tmp1 = PyTuple_GET_ITEM(args, 0);                           \
            PyObject *tmp2 = PyTuple_GET_ITEM(args, 1);                           \
            Py_INCREF(new1);                  Py_INCREF(new2);                    \
            PyTuple_SET_ITEM(args, 0, new1);  PyTuple_SET_ITEM(args, 1, new2);    \
            Py_XDECREF(tmp1);                 Py_XDECREF(tmp2);                   \
        } else {                                                                  \
            /* Create a new tuple and insert the newarg. */                       \
            PyObject *tmp = (args);                                               \
            args = PyTuple_New(2);                                                \
            if (args == NULL) {                                                   \
                error_stmt;                                                       \
            }                                                                     \
            Py_INCREF(new1);                 Py_INCREF(new2);                     \
            PyTuple_SET_ITEM(args, 0, new1); PyTuple_SET_ITEM(args, 1, new2);     \
            Py_DECREF(tmp);                                                       \
        }                                                                         \
    } while (0)

/******************************************************************************
 * Create a new reversed tuple from another tuple.
 *****************************************************************************/

PyObject *
PyUI_TupleReverse(PyObject *tuple)
{
    PyObject *item, *newtuple;
    Py_ssize_t num, idx;

    num = PyTuple_Size(tuple);
    newtuple = PyTuple_New(num);

    for (idx=0 ; idx<num ; idx++) {
        item = PyTuple_GET_ITEM(tuple, idx);
        Py_INCREF(item);
        PyTuple_SET_ITEM(newtuple, num-idx-1, item);
    }

    return newtuple;
}

/******************************************************************************
 * Copy a tuple. This is necessary because PyTuple_GetSlice doesn't return a
 * copy when the range is identical (or bigger) than the original tuple.
 *
 * tuple : Tuple where the value should be inserted.
 *****************************************************************************/

static PyObject *
PYUI_TupleCopy(PyObject *tuple)
{
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);
    Py_ssize_t i;
    PyObject *newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }
    for ( i=0 ; i<tuplesize ; i++) {
        PyObject *tmp = PyTuple_GET_ITEM(tuple, i);
        Py_INCREF(tmp);
        PyTuple_SET_ITEM(newtuple, i, tmp);
    }
    return newtuple;
}

/******************************************************************************
 * Insert a value in a Tuple by moving all items at or above this index one to
 * the right.
 *
 * WARNING: The last item of the Tuple mustn't be a PyObject or the caller must
 * have a reference to it - because this would leave a dangling reference!
 *
 * tuple : Tuple where the value should be inserted.
 * where : index to insert the value
 * v     : Value to insert
 * num   : Move items up to this index. I.e. if 10 then item 9 is moved to
 *         index 10 but item 10 isn't moved. (In fact item 10 mustn't be a
 *         PyObject, see Warning.)
 *****************************************************************************/

void
PYUI_TupleInsert(PyObject *tuple,
                 Py_ssize_t where,
                 PyObject *v,
                 Py_ssize_t num)
{
    /* Temporary variables */
    PyObject *temp;
    Py_ssize_t i;

    /* Move each of them to the next place, starting by the next-to-last
       element going left until where.
       */
    for (i=num-2 ; i>=where ; i--) {
        temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i+1, temp);
    }
    /* Insert the new element. */
    PyTuple_SET_ITEM(tuple, where, v);
}

/******************************************************************************
 * Remove a value from a Tuple and move every successive element one to the
 * left.
 *
 * WARNING: The value that is to be removed is not DECREF'd so the caller must
 * ensure that he DECREFs the removed item afterwards, otherwise this will
 * create a memory leak!
 *
 * tuple : Tuple where the value should be removed.
 * where : index where to remove the value
 * num   : Move items to up to this index. I.e. if num=10 then the item at pos
 *         10 is moved to 9 (and 10 is set to NULL), ... until where+1 which is
 *         moved to "where".
 *****************************************************************************/

void
PYUI_TupleRemove(PyObject *tuple,
                 Py_ssize_t where,
                 Py_ssize_t num)
{
    /* Temporary variables */
    PyObject *temp;
    Py_ssize_t idx;

    /* Move each item to the left from the after-where index until the end of
       the array.
       */
    for (idx = where + 1 ; idx < num ; idx++) {
        temp = PyTuple_GET_ITEM(tuple, idx);
        PyTuple_SET_ITEM(tuple, idx-1, temp);
    }
    /* Insert NULL at the last position. */
    PyTuple_SET_ITEM(tuple, num-1, NULL);
}

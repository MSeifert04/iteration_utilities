/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "helper.h"

/******************************************************************************
 * Global constants.
 *
 * Python objects that are created only once and stay in memory.
 *****************************************************************************/

PyObject *PyIU_global_zero = NULL;
PyObject *PyIU_global_one = NULL;
PyObject *PyIU_global_two = NULL;
PyObject *PyIU_global_0tuple = NULL;

void PyIU_InitializeConstants(void) {
    if (PyIU_global_zero == NULL) {
        PyIU_global_zero = PyLong_FromLong(0L);
        PyIU_global_one = PyLong_FromLong(1L);
        PyIU_global_two = PyLong_FromLong(2L);
        PyIU_global_0tuple = PyTuple_New(0);
    }
}

/******************************************************************************
 * Create a new tuple containing iterators for the input-tuple.
 *****************************************************************************/

PyObject *
PyIU_CreateIteratorTuple(PyObject *tuple) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));

    PyObject *newtuple;
    Py_ssize_t i;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0; i < tuplesize; i++) {
        PyObject *iterator = PyObject_GetIter(PyTuple_GET_ITEM(tuple, i));
        if (iterator == NULL) {
            Py_DECREF(newtuple);
            return NULL;
        }
        PyTuple_SET_ITEM(newtuple, i, iterator);
    }

    return newtuple;
}

/******************************************************************************
 * Create a new reversed tuple from another tuple.
 *****************************************************************************/

PyObject *
PyIU_TupleReverse(PyObject *tuple) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));

    PyObject *newtuple;
    Py_ssize_t i;
    Py_ssize_t j;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0, j = tuplesize - 1; i < tuplesize; i++, j--) {
        PyObject *item = PyTuple_GET_ITEM(tuple, i);
        Py_INCREF(item);
        PyTuple_SET_ITEM(newtuple, j, item);
    }

    return newtuple;
}

/******************************************************************************
 * Copy a tuple. This is necessary because PyTuple_GetSlice doesn't return a
 * copy when the range is identical (or bigger) than the original tuple.
 *
 * tuple : Tuple where the value should be inserted.
 *****************************************************************************/

PyObject *
PyIU_TupleCopy(PyObject *tuple) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));

    PyObject *newtuple;
    Py_ssize_t i;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0; i < tuplesize; i++) {
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

void PyIU_TupleInsert(PyObject *tuple, Py_ssize_t where, PyObject *v, Py_ssize_t num) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));
    assert(where >= 0 && where < PyTuple_GET_SIZE(tuple));
    assert(v != NULL);
    assert(num >= 0 && num <= PyTuple_GET_SIZE(tuple));

    Py_ssize_t i;

    /* Move each of them to the next place, starting by the next-to-last
       element going left until where.
       */
    for (i = num - 2; i >= where; i--) {
        PyObject *temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i + 1, temp);
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

void PyIU_TupleRemove(PyObject *tuple, Py_ssize_t where, Py_ssize_t num) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));
    assert(where >= 0 && where < PyTuple_GET_SIZE(tuple));
    assert(num >= 0 && num <= PyTuple_GET_SIZE(tuple));

    Py_ssize_t i;

    /* Move each item to the left from the after-where index until the end of
       the array.
       */
    for (i = where + 1; i < num; i++) {
        PyObject *temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i - 1, temp);
    }
    /* Insert NULL at the last position. */
    PyTuple_SET_ITEM(tuple, num - 1, NULL);
}

/******************************************************************************
 * Get the first 'n' values of a tuple.
 *
 * PyPy does not allow slicing tuples with NULL in it with PyTuple_GetSlice()
 * even if the NULL would not be copied. So this is put in a separate helper.
 *
 * tuple : Tuple to slice
 * num   : The number of items to copy from the tuple.
 *****************************************************************************/

PyObject *
PyIU_TupleGetSlice(PyObject *tuple, Py_ssize_t num) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));
    assert(num >= 0 && num < PyTuple_GET_SIZE(tuple));

    Py_ssize_t i;
    PyObject *result = PyTuple_New(num);
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < num; i++) {
        PyObject *tmp = PyTuple_GET_ITEM(tuple, i);
        assert(tmp != NULL);
        Py_INCREF(tmp);
        PyTuple_SET_ITEM(result, i, tmp);
    }
    return result;
}

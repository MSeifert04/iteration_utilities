/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

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
 * Convenience macros
 *
 * PYIU_CLEAR_STOPITERATION : Check if a StopIteration occurred and clear it.
 *****************************************************************************/

# define PYIU_CLEAR_STOPITERATION \
 if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_StopIteration)) { PyErr_Clear(); }

/******************************************************************************
 * Convenience macros
 *
 * PYIU_RECYCLE_ARG_TUPLE : args (Tuple of length 1)
 *                          newarg (PyObject *)
 *                          tmp (PyObject *)
 *                          error_stmt (for example "return NULL" or "goto Fail")
 *****************************************************************************/

#define PYIU_RECYCLE_ARG_TUPLE(args, newarg, tmp, error_stmt) \
    if (Py_REFCNT(args) == 1) { \
        /* Recycle args by replacing the element with newarg. */ \
        tmp = PyTuple_GET_ITEM(args, 0); \
        Py_INCREF(newarg); \
        PyTuple_SET_ITEM(args, 0, newarg); \
        Py_XDECREF(tmp); \
        tmp = NULL; \
    } else { \
        /* Create a new tuple and insert the newarg. */ \
        tmp = args; \
        args = PyTuple_New(1); \
        if (args == NULL) { \
            error_stmt; \
        } \
        Py_INCREF(newarg); \
        PyTuple_SET_ITEM(args, 0, newarg); \
        Py_DECREF(tmp); \
        tmp = NULL; \
    }


#define PYIU_RECYCLE_ARG_TUPLE_BINOP(args, new1, new2, tmp1, tmp2, error_stmt) \
    if (Py_REFCNT(args) == 1) { \
        /* Recycle args by replacing the element with newarg. */ \
        Py_INCREF(new1);                  Py_INCREF(new2); \
        tmp1 = PyTuple_GET_ITEM(args, 0); tmp2 = PyTuple_GET_ITEM(args, 1);\
        PyTuple_SET_ITEM(args, 0, new1);  PyTuple_SET_ITEM(args, 1, new2);\
        Py_XDECREF(tmp1);                 Py_XDECREF(tmp2); \
        tmp1 = NULL;                      tmp2 = NULL; \
    } else { \
        /* Create a new tuple and insert the newarg. */ \
        tmp1 = args; \
        args = PyTuple_New(2); \
        if (args == NULL) { \
            error_stmt; \
        } \
        Py_INCREF(new1);                 Py_INCREF(new2);\
        PyTuple_SET_ITEM(args, 0, new1); PyTuple_SET_ITEM(args, 1, new2); \
        Py_DECREF(tmp1); \
        tmp1 = NULL; \
    }

/******************************************************************************
 * Global constants.
 *
 * Python objects that are created only once and stay in memory:
 *
 * PyIU_LongTwo : 1
 * PyIU_LongTwo : 2
 *****************************************************************************/

static PyObject *PyIU_Long_1_global = NULL;

static PyObject * PyIU_Long_1(void) {
    if (PyIU_Long_1_global == NULL) {
#if PY_MAJOR_VERSION == 2
        PyIU_Long_1_global = PyInt_FromLong((long)1);
#else
        PyIU_Long_1_global = PyLong_FromLong((long)1);
#endif
    }
    return PyIU_Long_1_global;
}

static PyObject *PyIU_Long_2_global = NULL;

static PyObject * PyIU_Long_2(void) {
    if (PyIU_Long_2_global == NULL) {
#if PY_MAJOR_VERSION == 2
        PyIU_Long_2_global = PyInt_FromLong((long)2);
#else
        PyIU_Long_2_global = PyLong_FromLong((long)2);
#endif
    }
    return PyIU_Long_2_global;
}

/******************************************************************************
 *
 * Create a new reversed tuple from another tuple.
 *
 *****************************************************************************/

static PyObject * PyUI_TupleReverse(PyObject *tuple) {
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
 *
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
 *
 *****************************************************************************/

static void PYUI_TupleInsert(PyObject *tuple, Py_ssize_t where, PyObject *v,
                             Py_ssize_t num) {
    PyObject *temp;
    Py_ssize_t i;

    for (i=num-2 ; i>=where ; i--) {
        temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i+1, temp);
    }
    PyTuple_SET_ITEM(tuple, where, v);
}

/******************************************************************************
 *
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
 *
 *****************************************************************************/

static void PYUI_TupleRemove(PyObject *tuple, Py_ssize_t where,
                             Py_ssize_t num) {
    PyObject *temp;
    Py_ssize_t idx;

    for (idx = where + 1 ; idx < num ; idx++) {
        temp = PyTuple_GET_ITEM(tuple, idx);
        PyTuple_SET_ITEM(tuple, idx-1, temp);
    }
    PyTuple_SET_ITEM(tuple, num-1, NULL);
}



/******************************************************************************
 *
 * Find the position to insert a value in an already sorted tuple. Assumes that
 * the sorting should be stable and searches the rightmost place where the
 * tuple is still sorted.
 *
 * Function will compare first to the "hi-1"-th element and then start
 * bisecting. (See inline code for explanation).
 *
 * tuple : Sorted tuple to inspect
 * item  : Value to search the position for.
 * hi    : Upper index to search for.
 * cmpop : The comparison operator to use. For example Py_LT for a tuple sorted
 *         from low to high or Py_GT for a tuple sorted from high to low.
 *
 * Returns -1 on failure otherwise a positive Py_ssize_t value.
 *
 * Copied and modified from the python bisect module.
 *
 *****************************************************************************/

Py_ssize_t PyUI_TupleBisectRight_LastFirst(PyObject *tuple, PyObject *item,
                                           Py_ssize_t hi, int cmpop) {
    PyObject *litem;
    Py_ssize_t mid, lo = 0;
    int res;
    //printf("Start bisect right for %i elements.\n", hi);

    // Bisection has two worst cases: If it should be inserted in the first or
    // last place. The list is reverse-ordered so it's likely that the
    // bisection could return the last place (for bisect_left it would be the
    // first) in the "merge_sorted" function.

    // Checking the number of comparisons in "merge" shows that merge now uses
    // slightly less comparisons than "sorted" in the average case, slightly
    // more in the worst case and much less in the best case!

    // So let's check the last item first!
    if (hi <= 0) {
        return 0;
    }
    litem = PyTuple_GET_ITEM(tuple, hi-1);
    if (litem == NULL) {
        return -1;
    }
    res = PyObject_RichCompareBool(item, litem, cmpop);
    if (res == 1) {
        return hi;
    } else if (res == 0) {
        hi = hi - 1;
    } else {
        return -1;
    }


    while (lo < hi) {
        mid = ((size_t)lo + hi) / 2;
        //printf("mid: %i low: %i high: %i \n", mid, lo, hi);
        litem = PyTuple_GET_ITEM(tuple, mid);
        if (litem == NULL) {
            return -1;
        }
        res = PyObject_RichCompareBool(item, litem, cmpop);
        if (res == 1) {
            lo = mid + 1;
        } else if (res == 0) {
            hi = mid;
        } else {
            return -1;
        }
    }
    //printf("result: %i\n", lo);
    return lo;
}

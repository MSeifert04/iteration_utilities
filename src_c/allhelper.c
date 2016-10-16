/******************************************************************************
 * Check if a StopIteration occurred and clear it.
 *****************************************************************************/

# define PYIU_CLEAR_STOPITERATION \
 if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_StopIteration)) { PyErr_Clear(); }

/******************************************************************************
 * Global constants.
 *
 * Python objects that are created only once and stay in memory:
 *
 * PyIU_LongTwo : 2
 *****************************************************************************/

static PyObject *PyIU_Long_1_global = NULL;

static PyObject * PyIU_Long_1(void) {
    if (PyIU_Long_1_global == NULL) {
        PyIU_Long_1_global = PyLong_FromLong((long)1);
    }
    return PyIU_Long_1_global;
}

static PyObject *PyIU_Long_2_global = NULL;

static PyObject * PyIU_Long_2(void) {
    if (PyIU_Long_2_global == NULL) {
        PyIU_Long_2_global = PyLong_FromLong((long)2);
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
 * WARNING: The last item of the Tuple mustn't be a PyObject because this would
 * leave a dangling reference!
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
 * Find the position to insert a value in an already sorted tuple. Assumes that
 * the sorting should be stable and searches the rightmost place where the
 * tuple is still sorted.
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

Py_ssize_t PyUI_TupleBisectRight(PyObject *tuple, PyObject *item,
                                 Py_ssize_t hi, int cmpop) {
    PyObject *litem;
    Py_ssize_t mid, lo = 0;
    int res;

    while (lo < hi) {
        mid = ((size_t)lo + hi) / 2;
        litem = PyTuple_GET_ITEM(tuple, mid);
        if (litem == NULL) {
            return -1;
        }
        res = PyObject_RichCompareBool(item, litem, cmpop);
        if (res < 0) {
            return -1;
        }
        if (res) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}

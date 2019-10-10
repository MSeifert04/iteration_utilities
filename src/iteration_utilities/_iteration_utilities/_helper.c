/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
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
    return PyErr_SetString(PyExc_TypeError, "not implemented."), NULL

#define Py_UNUSED(name) _unused_ ## name
#endif

/******************************************************************************
 * Simple Convenience macros
 *
 * Set a value to NULL if it's None.
 *
 * PYIU_NULL_IF_NONE :
 *     arg : PyObject *
 *
 *****************************************************************************/

#define PYIU_NULL_IF_NONE(arg)     \
    do {                           \
        if (arg == Py_None) {      \
           arg = NULL;             \
        }                          \
    } while (0)


/******************************************************************************
 * Function call abstractions
 *
 * TODO: To support the different calling conventions across Python versions
 *
 * PyIU_CallWithOneArgument :
 *     Calls a function with one argument.
 *
 * PyIU_CallWithTwoArguments :
 *     Calls a function with two arguments.
 *****************************************************************************/


static PyObject*
PyIU_CallWithOneArgument(PyObject *callable, PyObject *arg1) {
    #if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8
        PyObject *args[1];
        args[0] = arg1;
        return _PyObject_Vectorcall(callable, args, 1, NULL);
    #elif PY_MAJOR_VERSION == 3 && (PY_MINOR_VERSION == 6 || PY_MINOR_VERSION == 7)
        PyObject *args[1];
        args[0] = arg1;
        return _PyObject_FastCall(callable, args, 1);
    #else
        PyObject *result;
        PyObject *args = PyTuple_New(1);
        if (args == NULL) {
            Py_DECREF(arg1);
            return NULL;
        }
        Py_INCREF(arg1);
        PyTuple_SET_ITEM(args, 0, arg1);
        result = PyObject_Call(callable, args, NULL);
        Py_DECREF(args);
        return result;
    #endif
}

static PyObject*
PyIU_CallWithTwoArguments(PyObject *callable, PyObject *arg1, PyObject *arg2) {
    #if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8
        PyObject *args[2];
        args[0] = arg1;
        args[1] = arg2;
        return _PyObject_Vectorcall(callable, args, 2, NULL);
    #elif PY_MAJOR_VERSION == 3 && (PY_MINOR_VERSION == 6 || PY_MINOR_VERSION == 7)
        PyObject *args[2];
        args[0] = arg1;
        args[1] = arg2;
        return _PyObject_FastCall(callable, args, 2);
    #else
        PyObject *result;
        PyObject *args = PyTuple_New(2);
        if (args == NULL) {
            return NULL;
        }
        Py_INCREF(arg1);
        Py_INCREF(arg2);
        PyTuple_SET_ITEM(args, 0, arg1);
        PyTuple_SET_ITEM(args, 1, arg2);
        result = PyObject_Call(callable, args, NULL);
        Py_DECREF(args);
        return result;
    #endif
}

/******************************************************************************
 * Global constants.
 *
 * Python objects that are created only once and stay in memory:
 *
 * PyIU_LongTwo : 1
 * PyIU_LongTwo : 2
 *
 * These are created in "_module.c"!
 *****************************************************************************/

static PyObject *PyIU_global_zero = NULL;
static PyObject *PyIU_global_one = NULL;
static PyObject *PyIU_global_two = NULL;
static PyObject *PyIU_global_0tuple = NULL;

/******************************************************************************
 * Create a new tuple containing iterators for the input-tuple.
 *****************************************************************************/

static PyObject *
PyIU_CreateIteratorTuple(PyObject *tuple)
{
    PyObject *newtuple;
    Py_ssize_t i;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0 ; i < tuplesize ; i++) {
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

static PyObject *
PyIU_TupleReverse(PyObject *tuple)
{
    PyObject *newtuple;
    Py_ssize_t i, j;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0, j = tuplesize-1 ; i < tuplesize ; i++, j--) {
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

static PyObject *
PyIU_TupleCopy(PyObject *tuple)
{
    PyObject *newtuple;
    Py_ssize_t i;
    Py_ssize_t tuplesize = PyTuple_GET_SIZE(tuple);

    newtuple = PyTuple_New(tuplesize);
    if (newtuple == NULL) {
        return NULL;
    }

    for (i = 0 ; i < tuplesize ; i++) {
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

static void
PyIU_TupleInsert(PyObject *tuple,
                 Py_ssize_t where,
                 PyObject *v,
                 Py_ssize_t num)
{
    /* Temporary variables */
    Py_ssize_t i;

    /* Move each of them to the next place, starting by the next-to-last
       element going left until where.
       */
    for (i = num-2 ; i >= where ; i--) {
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

static void
PyIU_TupleRemove(PyObject *tuple,
                 Py_ssize_t where,
                 Py_ssize_t num)
{
    /* Temporary variables */
    Py_ssize_t i;

    /* Move each item to the left from the after-where index until the end of
       the array.
       */
    for (i = where+1 ; i < num ; i++) {
        PyObject *temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i-1, temp);
    }
    /* Insert NULL at the last position. */
    PyTuple_SET_ITEM(tuple, num-1, NULL);
}

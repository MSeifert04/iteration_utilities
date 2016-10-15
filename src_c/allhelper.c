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

static PyObject* PyIU_Long_1(void) {
    if (PyIU_Long_1_global == NULL) {
        PyIU_Long_1_global = PyLong_FromLong((long)1);
    }
    return PyIU_Long_1_global;
}

static PyObject *PyIU_Long_2_global = NULL;

static PyObject* PyIU_Long_2(void) {
    if (PyIU_Long_2_global == NULL) {
        PyIU_Long_2_global = PyLong_FromLong((long)2);
    }
    return PyIU_Long_2_global;
}

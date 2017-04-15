/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_DotProduct(PyObject *m, PyObject *args)
{
    PyObject *vec1=NULL, *vec2=NULL;
    PyObject *iterator1=NULL, *iterator2=NULL;
    PyObject *item1=NULL, *item2=NULL;
    PyObject *product=NULL;
    PyObject *result=NULL;
    PyObject *tmp=NULL;

    if (!PyArg_ParseTuple(args, "OO", &vec1, &vec2)) {
        return NULL;
    }

    iterator1 = PyObject_GetIter(vec1);
    if (iterator1 == NULL) {
        goto Fail;
    }

    iterator2 = PyObject_GetIter(vec2);
    if (iterator2 == NULL) {
        goto Fail;
    }

    while ( (item1 = Py_TYPE(iterator1)->tp_iternext(iterator1)) &&
            (item2 = Py_TYPE(iterator2)->tp_iternext(iterator2))) {
        product = PyNumber_Multiply(item1, item2);
        if (product == NULL) {
            goto Fail;
        }

        if (result == NULL) {
            result = product;
            product = NULL;
        } else {
            tmp = result;
            result = PyNumber_Add(result, product);
            Py_DECREF(product);
            product = NULL;
            Py_DECREF(tmp);
            tmp = NULL;
            if (result == NULL) {
                goto Fail;
            }
        }

        Py_DECREF(item1);
        Py_DECREF(item2);
    }

    Py_DECREF(iterator1);
    Py_DECREF(iterator2);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_XDECREF(result);
            return NULL;
        }
    }

    if (result == NULL) {
        result = PyLong_FromLong((long)0);
    }
    return result;

Fail:
    Py_XDECREF(iterator1);
    Py_XDECREF(iterator2);
    Py_XDECREF(item1);
    Py_XDECREF(item2);
    Py_XDECREF(product);
    Py_XDECREF(result);
    return NULL;
}

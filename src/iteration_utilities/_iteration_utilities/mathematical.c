/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "mathematical.h"
#include "helper.h"

/******************************************************************************
 * partial-like functions:
 *
 * square     : lambda value: value ** 2
 * double     : lambda value: value * 2
 * reciprocal : lambda value: 1 / value
 *****************************************************************************/

PyObject *
PyIU_MathSquare(PyObject *Py_UNUSED(m), PyObject *o) {
    return PyNumber_Power(o, PyIU_global_two, Py_None);
}

PyObject *
PyIU_MathDouble(PyObject *Py_UNUSED(m), PyObject *o) {
    return PyNumber_Multiply(o, PyIU_global_two);
}

PyObject *
PyIU_MathReciprocal(PyObject *Py_UNUSED(m), PyObject *o) {
    return PyNumber_TrueDivide(PyIU_global_one, o);
}

/******************************************************************************
 * Reverse arithmetic operators:
 *
 * radd  : lambda o1, o2: o2 + o1
 * rsub  : lambda o1, o2: o2 - o1
 * rmul  : lambda o1, o2: o2 * o1
 * rdiv  : lambda o1, o2: o2 / o1
 * rfdiv : lambda o1, o2: o2 // o1
 * rpow  : lambda o1, o2: o2 ** o1
 * rmod  : lambda o1, o2: o2 % o1
 *****************************************************************************/

PyObject *
PyIU_MathRadd(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "radd", 2, 2, &op1, &op2)) {
        return PyNumber_Add(op2, op1);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRsub(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rsub", 2, 2, &op1, &op2)) {
        return PyNumber_Subtract(op2, op1);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRmul(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rmul", 2, 2, &op1, &op2)) {
        return PyNumber_Multiply(op2, op1);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRdiv(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rdiv", 2, 2, &op1, &op2)) {
        return PyNumber_TrueDivide(op2, op1);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRfdiv(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rfdiv", 2, 2, &op1, &op2)) {
        return PyNumber_FloorDivide(op2, op1);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRpow(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rpow", 2, 2, &op1, &op2)) {
        return PyNumber_Power(op2, op1, Py_None);
    } else {
        return NULL;
    }
}

PyObject *
PyIU_MathRmod(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *op1;
    PyObject *op2;
    if (PyArg_UnpackTuple(args, "rmod", 2, 2, &op1, &op2)) {
        return PyNumber_Remainder(op2, op1);
    } else {
        return NULL;
    }
}

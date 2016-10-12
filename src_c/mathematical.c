#include <Python.h>
#include <structmember.h>

/******************************************************************************
 * Wrapper functions for simple arithmetic functions.
 *
 * square   : identical to pow(x, 2)
 *****************************************************************************/

// global variable
static PyObject *mathematical_long_2 = NULL;
static PyObject *mathematical_long_1 = NULL;

// return global variable or if it's NULL initialize it.
static PyObject* mathematical_get_2(void) {
    if (mathematical_long_2 == NULL)
        mathematical_long_2 = PyLong_FromLong((long)2);

    return mathematical_long_2;
}

static PyObject* mathematical_get_1(void) {
    if (mathematical_long_1 == NULL)
        mathematical_long_1 = PyLong_FromLong((long)1);

    return mathematical_long_1;
}

// Functions
static PyObject*
mathematical_square(PyObject *self, PyObject *args) {
    return PyNumber_Power(args, mathematical_get_2(), Py_None);
}
static PyObject*
mathematical_double(PyObject *self, PyObject *args) {
    return PyNumber_Multiply(args, mathematical_get_2());
}
static PyObject*
mathematical_oneover(PyObject *self, PyObject *args) {
    return PyNumber_TrueDivide(mathematical_get_1(), args);
}

PyDoc_STRVAR(mathematical_square_doc,
"square(value)\n\
\n\
Returns the squared `value`.\n\
\n\
Parameters\n\
----------\n\
value : any type\n\
    The value to be squared. The type of the `value` must support ``pow``.\n\
\n\
Returns\n\
-------\n\
square : any type\n\
    Returns ``value**2``.\n\
\n\
Examples\n\
--------\n\
It is not possible to apply ``functools.partial`` to ``pow`` so that one has\n\
a one-argument square function and is significantly faster than ``lambda x: x**2``::\n\
\n\
    >>> from iteration_utilities import square\n\
    >>> square(1)\n\
    1\n\
    >>> square(2.0)\n\
    4.0\n\
");

PyDoc_STRVAR(mathematical_double_doc,
"double(value)\n\
\n\
Returns the doubled `value`.\n\
\n\
Parameters\n\
----------\n\
value : any type\n\
    The value to be doubled..\n\
\n\
Returns\n\
-------\n\
doubled : any type\n\
    Returns ``value*2``.\n\
\n\
Examples\n\
--------\n\
It is not possible to apply ``functools.partial`` to ``operator.mul`` so that one has\n\
a one-argument double function and is significantly faster than ``lambda x: x*2``::\n\
\n\
    >>> from iteration_utilities import double\n\
    >>> double(1)\n\
    2\n\
    >>> double(2.0)\n\
    4.0\n\
");

PyDoc_STRVAR(mathematical_oneover_doc,
"one_over(value)\n\
\n\
Returns ``1/value``.\n\
\n\
Parameters\n\
----------\n\
value : any type\n\
    The value for the computation.\n\
\n\
Returns\n\
-------\n\
one_over : any type\n\
    Returns ``1/value``.\n\
\n\
Examples\n\
--------\n\
It is not possible to apply ``functools.partial`` to ``operator.true_divide``\n\
so that one has a one-argument divide function and is significantly faster\n\
than ``lambda x: 1/x``::\n\
\n\
    >>> from iteration_utilities import one_over\n\
    >>> one_over(1)\n\
    1.0\n\
    >>> one_over(2)\n\
    0.5\n\
    >>> one_over(4)\n\
    0.25\n\
");


static PyObject*
mathematical_radd(PyObject *self, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "radd", 2, 2, &op1, &op2)) {
        return PyNumber_Add(op2, op1);
    } else {
        return NULL;
    }
}


static PyObject*
mathematical_rsub(PyObject *self, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rsub", 2, 2, &op1, &op2)) {
        return PyNumber_Subtract(op2, op1);
    } else {
        return NULL;
    }
}


static PyObject*
mathematical_rmul(PyObject *self, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rmul", 2, 2, &op1, &op2)) {
        return PyNumber_Multiply(op2, op1);
    } else {
        return NULL;
    }
}


static PyObject*
mathematical_rdiv(PyObject *self, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rtruediv", 2, 2, &op1, &op2)) {
        return PyNumber_TrueDivide(op2, op1);
    } else {
        return NULL;
    }
}


static PyObject*
mathematical_rpow(PyObject *self, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rpow", 2, 2, &op1, &op2)) {
        return PyNumber_Power(op2, op1, Py_None);
    } else {
        return NULL;
    }
}


PyDoc_STRVAR(mathematical_radd_doc,
"radd(op1, op2)\n\
\n\
Returns ``op2 + op1``.\n\
\n\
Parameters\n\
----------\n\
op1, op2 : any type\n\
    The values to be added.\n\
\n\
Returns\n\
-------\n\
radd : any type\n\
    Returns ``op2 + op1``.\n\
\n\
Examples\n\
--------\n\
Equivalent to ``lambda x, y: y + x``::\n\
\n\
    >>> from iteration_utilities import radd\n\
    >>> radd(2, 2)\n\
    4\n\
");


PyDoc_STRVAR(mathematical_rsub_doc,
"rsub(op1, op2)\n\
\n\
Returns ``op2 - op1``.\n\
\n\
Parameters\n\
----------\n\
op1, op2 : any type\n\
    The values to be subtracted.\n\
\n\
Returns\n\
-------\n\
rsub : any type\n\
    Returns ``op2 - op1``.\n\
\n\
Examples\n\
--------\n\
Equivalent to ``lambda x, y: y - x``::\n\
\n\
    >>> from iteration_utilities import rsub\n\
    >>> rsub(2, 5)\n\
    3\n\
");


PyDoc_STRVAR(mathematical_rmul_doc,
"rmul(op1, op2)\n\
\n\
Returns ``op2 * op1``.\n\
\n\
Parameters\n\
----------\n\
op1, op2 : any type\n\
    The values to be multiplied.\n\
\n\
Returns\n\
-------\n\
rmul : any type\n\
    Returns ``op2 * op1``.\n\
\n\
Examples\n\
--------\n\
Equivalent to ``lambda x, y: y * x``::\n\
\n\
    >>> from iteration_utilities import rmul\n\
    >>> rmul(2, 2)\n\
    4\n\
");


PyDoc_STRVAR(mathematical_rdiv_doc,
"rdiv(op1, op2)\n\
\n\
Returns ``op2 / op1``.\n\
\n\
Parameters\n\
----------\n\
op1, op2 : any type\n\
    The values to be divided.\n\
\n\
Returns\n\
-------\n\
rdiv : any type\n\
    Returns ``op2 / op1``.\n\
\n\
Examples\n\
--------\n\
Equivalent to ``lambda x, y: y / x``::\n\
\n\
    >>> from iteration_utilities import rdiv\n\
    >>> rdiv(10, 1)\n\
    0.1\n\
");


PyDoc_STRVAR(mathematical_rpow_doc,
"rpow(op1, op2)\n\
\n\
Returns ``op2 ** op1``.\n\
\n\
Parameters\n\
----------\n\
op1, op2 : any type\n\
    The values for the operation.\n\
\n\
Returns\n\
-------\n\
rpow : any type\n\
    Returns ``op2 ** op1``.\n\
\n\
Examples\n\
--------\n\
Equivalent to ``lambda x, y: y ** x``::\n\
\n\
    >>> from iteration_utilities import rpow\n\
    >>> rpow(3, 2)\n\
    8\n\
");

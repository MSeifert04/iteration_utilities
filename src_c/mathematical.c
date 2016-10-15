/******************************************************************************
 * square : equivalent to:
 *
 * lambda value: value ** 2
 *****************************************************************************/

static PyObject* PyIU_MathSquare(PyObject *m, PyObject *o) {
    return PyNumber_Power(o, PyIU_Long_2(), Py_None);
}

/******************************************************************************
 * double : equivalent to:
 *
 * lambda value: value * 2
 *****************************************************************************/

static PyObject* PyIU_MathDouble(PyObject *m, PyObject *o) {
    return PyNumber_Multiply(o, PyIU_Long_2());
}

/******************************************************************************
 * one_over : equivalent to:
 *
 * lambda value: 1 / value
 *****************************************************************************/

static PyObject* PyIU_MathOneOver(PyObject *m, PyObject *o) {
    return PyNumber_TrueDivide(PyIU_Long_1(), o);
}

/******************************************************************************
 * radd : equivalent to:
 *
 * lambda o1, o2: o2 + o1
 *****************************************************************************/

static PyObject* PyIU_MathRadd(PyObject *m, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "radd", 2, 2, &op1, &op2)) {
        return PyNumber_Add(op2, op1);
    } else {
        return NULL;
    }
}

/******************************************************************************
 * rsub : equivalent to:
 *
 * lambda o1, o2: o2 - o1
 *****************************************************************************/

static PyObject* PyIU_MathRsub(PyObject *m, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rsub", 2, 2, &op1, &op2)) {
        return PyNumber_Subtract(op2, op1);
    } else {
        return NULL;
    }
}

/******************************************************************************
 * rmul : equivalent to:
 *
 * lambda o1, o2: o2 * o1
 *****************************************************************************/

static PyObject* PyIU_MathRmul(PyObject *m, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rmul", 2, 2, &op1, &op2)) {
        return PyNumber_Multiply(op2, op1);
    } else {
        return NULL;
    }
}

/******************************************************************************
 * rdiv : equivalent to:
 *
 * lambda o1, o2: o2 / o1
 *****************************************************************************/

static PyObject* PyIU_MathRdiv(PyObject *m, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rdiv", 2, 2, &op1, &op2)) {
        return PyNumber_TrueDivide(op2, op1);
    } else {
        return NULL;
    }
}

/******************************************************************************
 * rpow : equivalent to:
 *
 * lambda o1, o2: o2 ** o1
 *****************************************************************************/

static PyObject* PyIU_MathRpow(PyObject *m, PyObject *args) {
    PyObject *op1, *op2;
    if (PyArg_UnpackTuple(args, "rpow", 2, 2, &op1, &op2)) {
        return PyNumber_Power(op2, op1, Py_None);
    } else {
        return NULL;
    }
}

/******************************************************************************
 *
 * Documentation
 *
 *****************************************************************************/

PyDoc_STRVAR(PyIU_MathSquare_doc,
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

PyDoc_STRVAR(PyIU_MathDouble_doc,
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

PyDoc_STRVAR(PyIU_MathOneOver_doc,
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

PyDoc_STRVAR(PyIU_MathRadd_doc,
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

PyDoc_STRVAR(PyIU_MathRsub_doc,
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

PyDoc_STRVAR(PyIU_MathRmul_doc,
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

PyDoc_STRVAR(PyIU_MathRdiv_doc,
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

PyDoc_STRVAR(PyIU_MathRpow_doc,
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

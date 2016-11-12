/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 * return_identity : lambda o: o
 * return_called : lambda o: o()
 * return_first_arg : (roughly) lambda *args, **kwargs: args[0]
 *****************************************************************************/

static PyObject *
PyIU_ReturnIdentity(PyObject *m,
                    PyObject *o)
{
    Py_INCREF(o);
    return o;
}

static PyObject *
PyIU_ReturnCalled(PyObject *m,
                  PyObject *o)
{
    return PyObject_CallFunctionObjArgs(o, NULL);
}

static PyObject *
PyIU_ReturnFirstArg(PyObject *m,
                    PyObject *args,
                    PyObject *kwargs)
{
    PyObject *first;

    if (!PyTuple_Check(args) || PyTuple_Size(args) == 0) {
        PyErr_Format(PyExc_TypeError,
                     "Expected at least one positional argument.");
        return NULL;
    }

    first = PyTuple_GET_ITEM(args, 0);
    Py_INCREF(first);
    return first;
}

/******************************************************************************
 * Documentation
 *****************************************************************************/

PyDoc_STRVAR(PyIU_ReturnIdentity_doc, "return_identity(obj, /)\n\
--\n\
\n\
Always return the argument.\n\
\n\
Parameters\n\
----------\n\
obj : any type \n\
    The `obj` to return.\n\
\n\
Returns\n\
-------\n\
identity : any type\n\
    The argument itself.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: x`` but significantly faster::\n\
\n\
    >>> from iteration_utilities import return_identity\n\
    >>> return_identity(1)\n\
    1\n\
    >>> return_identity('abc')\n\
    'abc'");

PyDoc_STRVAR(PyIU_ReturnCalled_doc, "return_called(func, /)\n\
--\n\
\n\
Return the result of ``func()``.\n\
\n\
Parameters\n\
----------\n\
func : callable \n\
    The function to be called.\n\
\n\
Returns\n\
-------\n\
result : any type\n\
    The result of ``func()``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: x()`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_called\n\
    >>> return_called(int)\n\
    0");

PyDoc_STRVAR(PyIU_ReturnFirstArg_doc, "return_first_arg(*args, **kwargs)\n\
--\n\
\n\
Always return the first positional argument given to the function.\n\
\n\
Parameters\n\
----------\n\
args, kwargs \n\
    any number of positional or keyword parameter.\n\
\n\
Returns\n\
-------\n\
first_positional_argument : any type\n\
    Always returns the first positional argument given to the function.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda *args, **kwargs: args[0]`` but\n\
significantly faster::\n\
\n\
    >>> from iteration_utilities import return_first_arg\n\
    >>> return_first_arg(1, 2, 3, 4, a=100)\n\
    1");

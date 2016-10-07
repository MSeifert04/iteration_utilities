/******************************************************************************
 * Functions always returning the same value.
 *****************************************************************************/

static PyObject*
returnx_returnIt(PyObject *self, PyObject *it) {
    Py_INCREF(it);
    return it;
}

PyDoc_STRVAR(returnx_returnIt_doc,
"return_identity(obj)\n\
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
    'abc'\n\
");

static PyObject*
returnx_returnFirstPositionalArgument(PyObject *self, PyObject *args, PyObject *keywds) {
    PyObject *first;
    Py_ssize_t tuple_size = PyTuple_Size(args);

    if (tuple_size == 0) {
        PyErr_Format(PyExc_TypeError, "Expected at least one positional argument.");
        return NULL;
    }

    first = PyTuple_GetItem(args, 0);
    if (first == NULL)
        return NULL;

    Py_INCREF(first);
    return first;
}

PyDoc_STRVAR(returnx_returnFirstPositionalArgument_doc,
"return_first_positional_argument(*args, **kwargs)\n\
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
This function is equivalent to ``lambda *args, **kwargs: args[0]`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import return_first_positional_argument\n\
    >>> return_first_positional_argument(1, 2, 3, 4, a=100)\n\
    1\n\
");

static PyObject*
returnx_returnCallResult(PyObject *self, PyObject *args) {
    return PyObject_CallFunctionObjArgs(args, NULL);
}

PyDoc_STRVAR(returnx_returnCallResult_doc,
"return_called(func)\n\
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
    0\n\
");

/******************************************************************************
 * Functions comparing to a specific value.
 *
 * isNone   : compare to None
 *****************************************************************************/

static PyObject*
isx_IsNone(PyObject *self, PyObject *args) {
    if (args == Py_None)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyDoc_STRVAR(isx_IsNone_doc,
"is_None(value)\n\
\n\
Returns ``True`` if `value` is ``None``, otherwise ``False``.\n\
\n\
Parameters\n\
----------\n\
value : any type \n\
    The value to test for ``None``.\n\
\n\
Returns\n\
-------\n\
is_none : bool\n\
    ``True`` if `value` is ``None`` otherwise it returns ``False``.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to ``lambda x: x is None`` but significantly\n\
faster::\n\
\n\
    >>> from iteration_utilities import is_None\n\
    >>> is_None(None)\n\
    True\n\
    >>> is_None(False)\n\
    False\n\
\n\
This can be used for example to remove all ``None`` from an iterable::\n\
\n\
    >>> import sys\n\
    >>> import itertools\n\
    >>> filterfalse = itertools.ifilterfalse if sys.version_info.major == 2 else itertools.filterfalse\n\
    >>> list(filterfalse(is_None, [1, None, 3, 4, 5, None, 7]))\n\
    [1, 3, 4, 5, 7]\n\
");

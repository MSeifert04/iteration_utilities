void helper_ExceptionClearStopIter()
{
    if (PyErr_Occurred() &&
        PyErr_ExceptionMatches(PyExc_StopIteration))
        PyErr_Clear();
}

#include <Python.h>
#include <structmember.h>

//Actual module method definition - this is the code that will be called by
//hello_module.print_hello_world
static PyObject*
simplecallbacks_returnTrue(PyObject *self, PyObject *args, PyObject *keywds)
{
    Py_INCREF(Py_True);
    return Py_True;
}

//Method definition object for this extension, these argumens mean:
//ml_name: The name of the method
//ml_meth: Function pointer to the method implementation
//ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
//ml_doc:  Contents of this method's docstring
static PyMethodDef
simplecallback_methods[] = {
    {
        "returnTrue",
        (PyCFunction)simplecallbacks_returnTrue,
        METH_VARARGS | METH_KEYWORDS,
        "Return ``True``."
    },
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
  //Module definition
  //The arguments of this structure tell Python what to call your extension,
  //what it's methods are and where to look for it's method definitions
  static struct PyModuleDef
  simplecallback_definition = {
    PyModuleDef_HEAD_INIT,
    "hello_module",
    "A Python module that prints 'hello world' from C code.",
    -1,
    simplecallback_methods
  };

  //Module initialization
  //Python calls this function when importing your extension. It is important
  //that this function is named PyInit_[[your_module_name]] exactly, and matches
  //the name keyword argument in setup.py's setup() call.
  PyMODINIT_FUNC
  PyInit_simplecallbacks(void)
  {
    Py_Initialize();
    return PyModule_Create(&simplecallback_definition);
  }
#else
  void
  initsimplecallbacks(void)
  {
    /* Create the module and add the functions */
    Py_InitModule("simplecallbacks", simplecallback_methods);
  }
#endif
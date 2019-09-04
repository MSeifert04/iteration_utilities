/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *item;
} PyIUObject_Constant;

static PyTypeObject PyIUType_Constant;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
constant_new(PyTypeObject *type,
             PyObject *args,
             PyObject *kwargs)
{
    PyIUObject_Constant *self;

    PyObject *item;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "constant", 1, 1, &item)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Constant *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(item);
    self->item = item;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
constant_dealloc(PyIUObject_Constant *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->item);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
constant_traverse(PyIUObject_Constant *self,
                  visitproc visit,
                  void *arg)
{
    Py_VISIT(self->item);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
constant_clear(PyIUObject_Constant *self)
{
    Py_CLEAR(self->item);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
constant_call(PyIUObject_Constant *self,
              PyObject *args,
              PyObject *kwargs)
{
    Py_INCREF(self->item);
    return self->item;
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
constant_repr(PyIUObject_Constant *self)
{
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->item);

    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
constant_reduce(PyIUObject_Constant *self,
                PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->item);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef constant_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)constant_reduce,                      /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Constant, x)
static PyMemberDef constant_memberlist[] = {

    {"item",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(item),                                         /* offset */
     READONLY,                                          /* flags */
     constant_prop_item_doc                             /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

static PyTypeObject PyIUType_Constant = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.constant",       /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Constant),            /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)constant_dealloc,                       /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)constant_repr,                            /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)constant_call,                         /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)constant_doc,                         /* tp_doc */
    (traverseproc)constant_traverse,                    /* tp_traverse */
    (inquiry)constant_clear,                            /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    constant_methods,                                   /* tp_methods */
    constant_memberlist,                                /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)constant_new,                              /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};

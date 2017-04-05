/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Flip;

PyTypeObject PyIUType_Flip;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
flip_new(PyTypeObject *type,
         PyObject *args,
         PyObject *kwargs)
{
    PyIUObject_Flip *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "flip", 1, 1, &func)) {
        return NULL;
    }

    /* If the object is another flip we can simply return it's function because
       two flips are equivalent to no flip. However subclasses should be
       excluded from this behaviour so also check that the first argument is
       in fact "flip" and not a subclass.
       */
    if (Py_TYPE(func) == &PyIUType_Flip && type == &PyIUType_Flip) {
        PyObject *ret = ((PyIUObject_Flip *)func)->func;
        Py_INCREF(ret);
        return ret;
    }

    /* Create struct */
    self = (PyIUObject_Flip *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
flip_dealloc(PyIUObject_Flip *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
flip_traverse(PyIUObject_Flip *self,
              visitproc visit,
              void *arg)
{
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
flip_clear(PyIUObject_Flip *self)
{
    Py_CLEAR(self->func);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
flip_call(PyIUObject_Flip *self,
          PyObject *args,
          PyObject *kwargs)
{
    PyObject *result, *tmpargs;

    if (PyTuple_GET_SIZE(args) >= 2) {
        tmpargs = PyIU_TupleReverse(args);
        result = PyObject_Call(self->func, tmpargs, kwargs);
        Py_DECREF(tmpargs);
    } else {
        result = PyObject_Call(self->func, args, kwargs);
    }

    return result;
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
flip_repr(PyIUObject_Flip *self)
{
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->func);

    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
flip_reduce(PyIUObject_Flip *self,
            PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(self),
                         self->func);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef flip_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)flip_reduce,                          /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Flip, x)
static PyMemberDef flip_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     flip_prop_func_doc                                 /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Flip = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.flip",           /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Flip),                /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)flip_dealloc,                           /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)flip_repr,                                /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)flip_call,                             /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)flip_doc,                             /* tp_doc */
    (traverseproc)flip_traverse,                        /* tp_traverse */
    (inquiry)flip_clear,                                /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    flip_methods,                                       /* tp_methods */
    flip_memberlist,                                    /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)flip_new,                                  /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};

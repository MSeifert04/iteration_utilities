/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

PyObject PlaceholderStruct;

#define PYIU_Placeholder (&PlaceholderStruct)


static PyObject *
placeholder_repr(PyObject *self)
{
    return PyUnicode_FromString("Placeholder");
}

static PyObject *
placeholder_reduce(PyObject *self)
{
    return PyUnicode_FromString("iteration_utilities.Placeholder");
}

static PyMethodDef placeholder_methods[] = {
    {"__reduce__", (PyCFunction)placeholder_reduce, METH_NOARGS, NULL},
    {NULL, NULL}
};

static PyObject *
placeholder_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    if (PyTuple_GET_SIZE(args) || (kwargs && PyDict_Size(kwargs))) {
        PyErr_SetString(PyExc_TypeError, "PlaceholderType takes no arguments");
        return NULL;
    }
    Py_INCREF(PYIU_Placeholder);
    return PYIU_Placeholder;
}

PyDoc_STRVAR(placeholder_doc, "PlaceholderType()\n\
--\n\
\n\
A placeholder for :py:func:`iteration_utilities.partial`.");

PyTypeObject Placeholder_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.PlaceholderType",
    0,
    0,
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_reserved*/
    placeholder_repr,          /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call */
    0,                         /*tp_str */
    0,                         /*tp_getattro */
    0,                         /*tp_setattro */
    0,                         /*tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /*tp_flags */
    placeholder_doc,           /*tp_doc */
    0,                         /*tp_traverse */
    0,                         /*tp_clear */
    0,                         /*tp_richcompare */
    0,                         /*tp_weaklistoffset */
    0,                         /*tp_iter */
    0,                         /*tp_iternext */
    placeholder_methods,       /*tp_methods */
    0,                         /*tp_members */
    0,                         /*tp_getset */
    0,                         /*tp_base */
    0,                         /*tp_dict */
    0,                         /*tp_descr_get */
    0,                         /*tp_descr_set */
    0,                         /*tp_dictoffset */
    0,                         /*tp_init */
    0,                         /*tp_alloc */
    placeholder_new,           /*tp_new */
};

PyObject PlaceholderStruct = {
    _PyObject_EXTRA_INIT
    1, &Placeholder_Type
};


/******************************************************************************
 * The following code is has a different license:
 *
 * PSF license - see licenses/LICENSE_PYTHON.rst
 *
 * These functions are _roughly_ identical to the one in "functoolsmodule.c" in
 * CPython.
 *
 * Changes with respect to the original Python functions are licensed under
 * the "iteration_utilities" license:
 * Apache License Version 2.0 - see LICENSE.rst
 *
 *****************************************************************************/


typedef struct {
    PyObject_HEAD
    PyObject *fn;
    PyObject *args;
    PyObject *kw;
    PyObject *dict;
    PyObject *weakreflist; /* List of weak references */
} PyIUObject_Partial;

PyTypeObject PyIUType_Partial;

static PyObject *
partial_new(PyTypeObject *type, PyObject *args, PyObject *kw)
{
    PyObject *func, *pargs, *nargs, *pkw;
    PyIUObject_Partial *self;

    if (PyTuple_GET_SIZE(args) < 1) {
        PyErr_SetString(PyExc_TypeError,
                        "type 'partial' takes at least one argument");
        return NULL;
    }

    pargs = pkw = NULL;
    func = PyTuple_GET_ITEM(args, 0);
    if (Py_TYPE(func) == &PyIUType_Partial && type == &PyIUType_Partial) {
        PyIUObject_Partial *part = (PyIUObject_Partial *)func;
        if (part->dict == NULL) {
            pargs = part->args;
            pkw = part->kw;
            func = part->fn;
        }
    }
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError,
                        "the first argument must be callable");
        return NULL;
    }

    /* create PyIUObject_Partial structure */
    self = (PyIUObject_Partial *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }

    self->fn = func;
    Py_INCREF(func);

    nargs = PyTuple_GetSlice(args, 1, PY_SSIZE_T_MAX);
    if (nargs == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    if (pargs == NULL || PyTuple_GET_SIZE(pargs) == 0) {
        self->args = nargs;
        Py_INCREF(nargs);
    }
    else if (PyTuple_GET_SIZE(nargs) == 0) {
        self->args = pargs;
        Py_INCREF(pargs);
    }
    else {
        self->args = PySequence_Concat(pargs, nargs);
        if (self->args == NULL) {
            Py_DECREF(nargs);
            Py_DECREF(self);
            return NULL;
        }
    }
    Py_DECREF(nargs);

    if (pkw == NULL || PyDict_Size(pkw) == 0) {
        if (kw == NULL) {
            self->kw = PyDict_New();
        }
        else if (Py_REFCNT(kw) == 1) {
            Py_INCREF(kw);
            self->kw = kw;
        }
        else {
            self->kw = PyDict_Copy(kw);
        }
    } else {
        self->kw = PyDict_Copy(pkw);
        if (kw != NULL && self->kw != NULL) {
            if (PyDict_Merge(self->kw, kw, 1) != 0) {
                Py_DECREF(self);
                return NULL;
            }
        }
    }

    if (self->kw == NULL) {
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject *)self;
}

static void
partial_dealloc(PyIUObject_Partial *self)
{
    PyObject_GC_UnTrack(self);
    if (self->weakreflist != NULL) {
        PyObject_ClearWeakRefs((PyObject *) self);
    }
    Py_XDECREF(self->fn);
    Py_XDECREF(self->args);
    Py_XDECREF(self->kw);
    Py_XDECREF(self->dict);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *
partial_call(PyIUObject_Partial *self, PyObject *args, PyObject *kw)
{
    PyObject *ret;
    PyObject *argappl, *kwappl;

    if (PyTuple_GET_SIZE(self->args) == 0) {
        argappl = args;
        Py_INCREF(args);
    } else if (PyTuple_GET_SIZE(args) == 0) {
        argappl = self->args;
        Py_INCREF(self->args);
    } else {
        argappl = PySequence_Concat(self->args, args);
        if (argappl == NULL) {
            return NULL;
        }
    }

    if (PyDict_Size(self->kw) == 0) {
        kwappl = kw;
        Py_XINCREF(kwappl);
    } else {
        kwappl = PyDict_Copy(self->kw);
        if (kwappl == NULL) {
            Py_DECREF(argappl);
            return NULL;
        }
        if (kw != NULL) {
            if (PyDict_Merge(kwappl, kw, 1) != 0) {
                Py_DECREF(argappl);
                Py_DECREF(kwappl);
                return NULL;
            }
        }
    }

    ret = PyObject_Call(self->fn, argappl, kwappl);
    Py_DECREF(argappl);
    Py_XDECREF(kwappl);
    return ret;
}

static int
partial_traverse(PyIUObject_Partial *self, visitproc visit, void *arg)
{
    Py_VISIT(self->fn);
    Py_VISIT(self->args);
    Py_VISIT(self->kw);
    Py_VISIT(self->dict);
    return 0;
}

#define OFF(x) offsetof(PyIUObject_Partial, x)
static PyMemberDef partial_memberlist[] = {
    {"func",            T_OBJECT,       OFF(fn),        READONLY,
     "function object to use in future partial calls"},
    {"args",            T_OBJECT,       OFF(args),      READONLY,
     "tuple of arguments to future partial calls"},
    {"keywords",        T_OBJECT,       OFF(kw),        READONLY,
     "dictionary of keyword arguments to future partial calls"},
    {NULL}  /* Sentinel */
};

#if PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3)
static PyObject *
partial_get_dict(PyIUObject_Partial *self)
{
    if (self->dict == NULL) {
        self->dict = PyDict_New();
        if (self->dict == NULL) {
            return NULL;
        }
    }
    Py_INCREF(self->dict);
    return self->dict;
}

static int
partial_set_dict(PyIUObject_Partial *self, PyObject *value)
{
    PyObject *tmp;

    /* It is illegal to del p.__dict__ */
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "a partial object's dictionary may not be deleted");
        return -1;
    }
    /* Can only set __dict__ to a dictionary */
    if (!PyDict_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "setting partial object's dictionary to a non-dict");
        return -1;
    }
    tmp = self->dict;
    Py_INCREF(value);
    self->dict = value;
    Py_XDECREF(tmp);
    return 0;
}

static PyGetSetDef partial_getsetlist[] = {
    {"__dict__", (getter)partial_get_dict, (setter)partial_set_dict, NULL},
    {NULL} /* Sentinel */
};
#else
static PyGetSetDef partial_getsetlist[] = {
    {"__dict__", PyObject_GenericGetDict, PyObject_GenericSetDict, NULL},
    {NULL} /* Sentinel */
};
#endif

static PyObject *
partial_repr(PyIUObject_Partial *self)
{
    PyObject *result = NULL;
    PyObject *arglist;
    Py_ssize_t i, n;
    PyObject *key, *value;
    int status;

    status = Py_ReprEnter((PyObject *)self);
    if (status != 0) {
        if (status < 0) {
            return NULL;
        }
        return PyUnicode_FromString("...");
    }

    arglist = PyUnicode_FromString("");
    if (arglist == NULL) {
        goto done;
    }

    /* Pack positional arguments */
    n = PyTuple_GET_SIZE(self->args);
    for (i = 0; i < n; i++) {
        PyObject *tmp = PyUnicode_FromFormat("%U, %R", arglist,
                                             PyTuple_GET_ITEM(self->args, i));
        Py_CLEAR(arglist);
        arglist = tmp;
        if (arglist == NULL) {
            goto done;
        }
    }

    /* Pack keyword arguments */
    i = 0;
    while (PyDict_Next(self->kw, &i, &key, &value)) {
        PyObject *tmp;
        /* This is mostly a special case because of Python2 which segfaults
           for normal strings when used as "%U" in "PyUnicode_FromFormat"
           However setstate also allows to pass in arbitary dictionaries
           with non-string keys. To prevent segfaults in that case this
           branch is also important for python3.
           */
        PyObject *othertmp = PyUnicode_FromObject(key);
        if (othertmp == NULL) {
            Py_DECREF(arglist);
            goto done;
        }
        tmp = PyUnicode_FromFormat("%U, %U=%R", arglist, othertmp, value);
        Py_DECREF(othertmp);

        Py_CLEAR(arglist);
        arglist = tmp;
        if (arglist == NULL) {
            goto done;
        }
    }

    result = PyUnicode_FromFormat("%s(%R%U)", Py_TYPE(self)->tp_name,
                                  self->fn, arglist);
    Py_DECREF(arglist);


done:
    Py_ReprLeave((PyObject *)self);
    return result;
}

/* Pickle strategy:
   __reduce__ by itself doesn't support getting kwargs in the unpickle
   operation so we define a __setstate__ that replaces all the information
   about the partial.  If we only replaced part of it someone would use
   it as a hook to do strange things.
 */

static PyObject *
partial_reduce(PyIUObject_Partial *self, PyObject *unused)
{
    return Py_BuildValue("O(O)(OOOO)", Py_TYPE(self), self->fn, self->fn,
                         self->args, self->kw,
                         self->dict ? self->dict : Py_None);
}

static PyObject *
partial_setstate(PyIUObject_Partial *self, PyObject *state)
{
    PyObject *fn, *fnargs, *kw, *dict;

    if (!PyTuple_Check(state) ||
            !PyArg_ParseTuple(state, "OOOO", &fn, &fnargs, &kw, &dict) ||
            !PyCallable_Check(fn) ||
            !PyTuple_Check(fnargs) ||
            (kw != Py_None && !PyDict_Check(kw))) {
        PyErr_SetString(PyExc_TypeError, "invalid partial state");
        return NULL;
    }

    if(!PyTuple_CheckExact(fnargs)) {
        fnargs = PySequence_Tuple(fnargs);
    }
    else {
        Py_INCREF(fnargs);
    }
    if (fnargs == NULL) {
        return NULL;
    }

    if (kw == Py_None) {
        kw = PyDict_New();
    } else if (!PyDict_CheckExact(kw)) {
        kw = PyDict_Copy(kw);
    } else {
        Py_INCREF(kw);
    }
    if (kw == NULL) {
        Py_DECREF(fnargs);
        return NULL;
    }

    Py_INCREF(fn);
    if (dict == Py_None) {
        dict = NULL;
    }
    else {
        Py_INCREF(dict);
    }

    Py_CLEAR(self->fn);
    Py_CLEAR(self->args);
    Py_CLEAR(self->kw);
    Py_CLEAR(self->dict);
    self->fn = fn;
    self->args = fnargs;
    self->kw = kw;
    self->dict = dict;
    Py_RETURN_NONE;
}

static PyMethodDef partial_methods[] = {
    {"__reduce__",   (PyCFunction)partial_reduce,   METH_NOARGS},
    {"__setstate__", (PyCFunction)partial_setstate, METH_O},
    {NULL,              NULL}
};


PyDoc_STRVAR(partial_doc, "partial(func, *args, **kwargs)\n\
--\n\
\n\
Almost like :py:func:`functools.partial`.");

PyTypeObject PyIUType_Partial = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.partial",                   /* tp_name */
    sizeof(PyIUObject_Partial),                      /* tp_basicsize */
    0,                                               /* tp_itemsize */
    /* methods */
    (destructor)partial_dealloc,                     /* tp_dealloc */
    0,                                               /* tp_print */
    0,                                               /* tp_getattr */
    0,                                               /* tp_setattr */
    0,                                               /* tp_reserved */
    (reprfunc)partial_repr,                          /* tp_repr */
    0,                                               /* tp_as_number */
    0,                                               /* tp_as_sequence */
    0,                                               /* tp_as_mapping */
    0,                                               /* tp_hash */
    (ternaryfunc)partial_call,                       /* tp_call */
    0,                                               /* tp_str */
    PyObject_GenericGetAttr,                         /* tp_getattro */
    PyObject_GenericSetAttr,                         /* tp_setattro */
    0,                                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                         /* tp_flags */
    partial_doc,                                     /* tp_doc */
    (traverseproc)partial_traverse,                  /* tp_traverse */
    0,                                               /* tp_clear */
    0,                                               /* tp_richcompare */
    offsetof(PyIUObject_Partial, weakreflist),       /* tp_weaklistoffset */
    0,                                               /* tp_iter */
    0,                                               /* tp_iternext */
    partial_methods,                                 /* tp_methods */
    partial_memberlist,                              /* tp_members */
    partial_getsetlist,                              /* tp_getset */
    0,                                               /* tp_base */
    0,                                               /* tp_dict */
    0,                                               /* tp_descr_get */
    0,                                               /* tp_descr_set */
    offsetof(PyIUObject_Partial, dict),              /* tp_dictoffset */
    0,                                               /* tp_init */
    0,                                               /* tp_alloc */
    partial_new,                                     /* tp_new */
    PyObject_GC_Del,                                 /* tp_free */
};

typedef struct {
    PyObject_HEAD
    PyObject *ittuple;
    Py_ssize_t numactive;
    Py_ssize_t active;
} recipes_roundrobin_object;


static PyObject *
recipes_roundrobin_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *ittuple;
    Py_ssize_t numactive;
    recipes_roundrobin_object *lz;

    PyObject *item, *iterator;
    Py_ssize_t i;

    assert(PyTuple_Check(args));
    numactive = PyTuple_Size(args);
    ittuple = PyTuple_New(numactive); //REFCNT(ittuple) = 1
    if (ittuple == NULL) {
        return NULL;
    }

    for (i = 0 ; i < numactive ; i++) {
        item = PyTuple_GET_ITEM(args, i); //REFCNT(item) = 0
        iterator = PyObject_GetIter(item); //REFCNT(iterator) = 1
        if (iterator == NULL) {
            Py_DECREF(ittuple);
            return NULL;
        }
        PyTuple_SET_ITEM(ittuple, i, iterator); //REFCNT(iterator) = 0
    }

    /* create recipes_roundrobin_object structure */
    lz = (recipes_roundrobin_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(ittuple);
        return NULL;
    }

    lz->ittuple = ittuple;
    lz->numactive = numactive;
    lz->active = 0;

    return (PyObject *)lz;
}


static void
recipes_roundrobin_dealloc(recipes_roundrobin_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->ittuple);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_roundrobin_traverse(recipes_roundrobin_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->ittuple);
    return 0;
}


static PyObject *
recipes_roundrobin_next(recipes_roundrobin_object *lz)
{
    PyObject *ittuple = lz->ittuple;
    Py_ssize_t numactive = lz->numactive;
    Py_ssize_t active = lz->active;
    Py_ssize_t i;

    PyObject *iterator, *item, *temp;

    if (lz->numactive == 0) {
        return NULL;
    }

    iterator = PyTuple_GET_ITEM(ittuple, active);
    while ((item = PyIter_Next(iterator)) == NULL) {
        if (active == lz->numactive -1) {
            PyTuple_SET_ITEM(ittuple, active, NULL);
            active = 0;
        } else {
            for (i = active + 1 ; i < lz->numactive ; i++) {
                temp = PyTuple_GET_ITEM(ittuple, i);
                PyTuple_SET_ITEM(ittuple, i - 1, temp);
            }
            PyTuple_SET_ITEM(ittuple, lz->numactive - 1, NULL);
        }
        lz->numactive--;
        Py_DECREF(iterator);
        if (lz->numactive == 0) {
            break;
        }
        iterator = PyTuple_GET_ITEM(ittuple, active);
    }
    if (lz->numactive == 0) {
        return NULL;
    }
    lz->active = (lz->active + 1) % (lz->numactive);
    return item;
}


static PyObject *
recipes_roundrobin_reduce(recipes_roundrobin_object *lz)
{
    PyObject *ittuple, *temp, *res;
    Py_ssize_t i;

    if (PyTuple_Size(lz->ittuple) != lz->numactive) {
        ittuple = PyTuple_New(lz->numactive);
        if (ittuple == NULL) {
            return NULL;
        }
        for (i=0 ; i<lz->numactive ; i++) {
            temp = PyTuple_GET_ITEM(lz->ittuple, i);
            Py_INCREF(temp);
            PyTuple_SET_ITEM(ittuple, i, temp);
        }
    } else {
        ittuple = lz->ittuple;
        Py_INCREF(ittuple);
    }
    res = Py_BuildValue("OO(nn)", Py_TYPE(lz),
                        ittuple,
                        lz->numactive, lz->active);
    Py_DECREF(ittuple);
    return res;
}

static PyObject *
recipes_roundrobin_setstate(recipes_roundrobin_object *lz, PyObject *state)
{
    Py_ssize_t numactive, active;
    if (!PyArg_ParseTuple(state, "nn", &numactive, &active)) {
        return NULL;
    }

    lz->numactive = numactive;
    lz->active = active;

    Py_RETURN_NONE;
}


static PyMethodDef recipes_roundrobin_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_roundrobin_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_roundrobin_setstate,
     METH_O,
     ""},

    {NULL,           NULL}           /* sentinel */
};


PyDoc_STRVAR(recipes_roundrobin_doc,
"roundrobin(*iterable)\n\
\n\
Round-Robin implementation ([0]_).\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    `Iterables` to combine using the round-robin. Any amount of iterables\n\
    are supported.\n\
\n\
Returns\n\
-------\n\
roundrobin : generator\n\
    Iterable filled with the values of the `iterables`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import roundrobin\n\
>>> list(roundrobin('ABC', 'D', 'EF'))\n\
['A', 'D', 'E', 'B', 'F', 'C']\n\
");

PyTypeObject recipes_roundrobin_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.roundrobin",   /* tp_name */
    sizeof(recipes_roundrobin_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_roundrobin_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    recipes_roundrobin_doc,             /* tp_doc */
    (traverseproc)recipes_roundrobin_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_roundrobin_next, /* tp_iternext */
    recipes_roundrobin_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    recipes_roundrobin_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
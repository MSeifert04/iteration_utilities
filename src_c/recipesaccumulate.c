/******************************************************************************
 * Part of recipes.c
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *total;
    PyObject *it;
    PyObject *binop;
} recipes_accumulateobject;

static PyTypeObject recipes_accumulate_type;

static PyObject *
recipes_accumulate_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "func", NULL};
    PyObject *iterable;
    PyObject *it;
    PyObject *binop = Py_None;
    recipes_accumulateobject *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O:accumulate",
                                     kwargs, &iterable, &binop))
        return NULL;

    /* Get iterator. */
    it = PyObject_GetIter(iterable);
    if (it == NULL)
        return NULL;

    /* create accumulateobject structure */
    lz = (recipes_accumulateobject *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    if (binop != Py_None) {
        Py_XINCREF(binop);
        lz->binop = binop;
    }
    lz->total = NULL;
    lz->it = it;
    return (PyObject *)lz;
}

static void
recipes_accumulate_dealloc(recipes_accumulateobject *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->binop);
    Py_XDECREF(lz->total);
    Py_XDECREF(lz->it);
    Py_TYPE(lz)->tp_free(lz);
}

static int
recipes_accumulate_traverse(recipes_accumulateobject *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->binop);
    Py_VISIT(lz->it);
    Py_VISIT(lz->total);
    return 0;
}

static PyObject *
recipes_accumulate_next(recipes_accumulateobject *lz)
{
    PyObject *val, *newtotal;

    val = (*Py_TYPE(lz->it)->tp_iternext)(lz->it);
    if (val == NULL)
        return NULL;

    if (lz->total == NULL) {
        Py_INCREF(val);
        lz->total = val;
        return lz->total;
    }

    if (lz->binop == NULL)
        newtotal = PyNumber_Add(lz->total, val);
    else
        newtotal = PyObject_CallFunctionObjArgs(lz->binop, lz->total, val, NULL);
    Py_DECREF(val);
    if (newtotal == NULL)
        return NULL;

    Py_INCREF(newtotal);
    Py_SETREF(lz->total, newtotal);
    return newtotal;
}

/*
static PyObject *
recipes_accumulate_reduce(recipes_accumulateobject *lz)
{
    if (lz->total == Py_None) {
        PyObject *it;

        if (PyType_Ready(&chain_type) < 0)
            return NULL;
        if (PyType_Ready(&islice_type) < 0)
            return NULL;
        it = PyObject_CallFunction((PyObject *)&chain_type, "(O)O",
                                   lz->total, lz->it);
        if (it == NULL)
            return NULL;
        it = PyObject_CallFunction((PyObject *)Py_TYPE(lz), "NO",
                                   it, lz->binop ? lz->binop : Py_None);
        if (it == NULL)
            return NULL;
        return Py_BuildValue("O(NiO)", &islice_type, it, 1, Py_None);
    }
    return Py_BuildValue("O(OO)O", Py_TYPE(lz),
                            lz->it, lz->binop?lz->binop:Py_None,
                            lz->total?lz->total:Py_None);
}

static PyObject *
recipes_accumulate_setstate(recipes_accumulateobject *lz, PyObject *state)
{
    Py_INCREF(state);
    Py_XSETREF(lz->total, state);
    Py_RETURN_NONE;
}

static PyMethodDef recipes_accumulate_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_accumulate_reduce,
     METH_NOARGS,
     reduce_doc},

    {"__setstate__",
     (PyCFunction)recipes_accumulate_setstate,
     METH_O,
     setstate_doc},

    {NULL,              NULL}   /* sentinel */
};
*/

static PyMethodDef recipes_accumulate_methods[] = {
    {NULL,              NULL}   /* sentinel */
};

PyDoc_STRVAR(recipes_accumulate_doc,
"accumulate(iterable[, func])\n\
\n\
Return series of accumulated sums (or other binary function results).");

static PyTypeObject recipes_accumulate_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.c_accumulate", /* tp_name */
    sizeof(recipes_accumulateobject),   /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_accumulate_dealloc, /* tp_dealloc */
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
    recipes_accumulate_doc,             /* tp_doc */
    (traverseproc)recipes_accumulate_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_accumulate_next, /* tp_iternext */
    recipes_accumulate_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_accumulate_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};

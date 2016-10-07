typedef struct {
    PyObject_HEAD
    PyObject *it;
    PyObject *keyfunc;
    PyObject *lastitem;
} recipes_uniquejust_object;

static PyTypeObject recipes_uniquejust_type;

static PyObject *
recipes_uniquejust_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "key", NULL};

    PyObject *iterable, *keyfunc=NULL;
    PyObject *it;

    recipes_uniquejust_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O:unique_justseen", kwargs,
                                     &iterable, &keyfunc)) {
        return NULL;
    }

    /* Get iterator. */
    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    if (keyfunc != NULL) {
        if (keyfunc == Py_None) {
            keyfunc = NULL;
        } else {
            Py_INCREF(keyfunc);
        }
    }

    /* create uniquejust structure */
    lz = (recipes_uniquejust_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        Py_XDECREF(keyfunc);
        return NULL;
    }

    lz->it = it;
    lz->keyfunc = keyfunc;
    lz->lastitem = NULL;

    return (PyObject *)lz;
}

static void
recipes_uniquejust_dealloc(recipes_uniquejust_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->keyfunc);
    Py_XDECREF(lz->lastitem);
    Py_TYPE(lz)->tp_free(lz);
}

static int
recipes_uniquejust_traverse(recipes_uniquejust_object *lz, visitproc visit,
                            void *arg)
{
    Py_VISIT(lz->it);
    Py_VISIT(lz->keyfunc);
    Py_VISIT(lz->lastitem);
    return 0;
}

static PyObject *
recipes_uniquejust_next(recipes_uniquejust_object *lz)
{
    PyObject *it = lz->it;
    PyObject *keyfunc = lz->keyfunc;

    int ok;

    PyObject *item, *val, *old;

    while ( (item = PyIter_Next(it)) ) {
        if (keyfunc == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            val = PyObject_CallFunctionObjArgs(keyfunc, item, NULL);
            if (val == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        }

        if (lz->lastitem == NULL) {
            lz->lastitem = val;
            return item;
        }

        ok = PyObject_RichCompareBool(val, lz->lastitem, Py_EQ);

        if (ok < 0) {
            Py_DECREF(val);
            Py_DECREF(item);
        }

        if (ok == 0) {
            old = lz->lastitem;
            Py_DECREF(old);
            lz->lastitem = val;
            return item;
        }

        Py_DECREF(val);
        Py_DECREF(item);
    }
    return NULL;
}

static PyObject *
recipes_uniquejust_reduce(recipes_uniquejust_object *lz) {
    PyObject *value;

    if (lz->lastitem != NULL) {
        value = Py_BuildValue("O(OO)(O)", Py_TYPE(lz),
                             lz->it,
                             lz->keyfunc ? lz->keyfunc : Py_None,
                             lz->lastitem ? lz->lastitem : Py_None);
    } else {
        value = Py_BuildValue("O(OO)", Py_TYPE(lz),
                             lz->it,
                             lz->keyfunc ? lz->keyfunc : Py_None);
    }

    return value;
}

static PyObject *
recipes_uniquejust_setstate(recipes_uniquejust_object *lz, PyObject *state)
{
    PyObject *lastitem;

    if (!PyArg_ParseTuple(state, "O", &lastitem)) {
        return NULL;
    }

    Py_CLEAR(lz->lastitem);
    lz->lastitem = lastitem;
    Py_INCREF(lz->lastitem);

    Py_RETURN_NONE;
}

static PyMethodDef recipes_uniquejust_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_uniquejust_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_uniquejust_setstate,
     METH_O,
     ""},

    {NULL,              NULL}   /* sentinel */
};

PyDoc_STRVAR(recipes_uniquejust_doc,
"unique_justseen(iterable[, key])\n\
\n\
List unique elements, preserving order. Remember only the element just \n\
seen.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` to check.\n\
\n\
key : callable or None, optional\n\
    If ``None`` the values are taken as they are. If it's a callable the\n\
    callable is applied to the value before comparing it.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
iterable : generator\n\
    An iterable containing all unique values just seen in the `iterable`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import unique_justseen\n\
>>> list(unique_justseen('AAAABBBCCDAABBB'))\n\
['A', 'B', 'C', 'D', 'A', 'B']\n\
\n\
>>> list(unique_justseen('ABBCcAD', str.lower))\n\
['A', 'B', 'C', 'A', 'D']\n\
");

static PyTypeObject recipes_uniquejust_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.unique_justseen", /* tp_name */
    sizeof(recipes_uniquejust_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_uniquejust_dealloc, /* tp_dealloc */
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
    recipes_uniquejust_doc,             /* tp_doc */
    (traverseproc)recipes_uniquejust_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_uniquejust_next, /* tp_iternext */
    recipes_uniquejust_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_uniquejust_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};

/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/


/******************************************************************************
 *
 * Helper class that mimics a 2-tuple when compared but dynamically decides
 * which item to compare (item or key) and assumes that the idx is always
 * different.
 *
 * It also has a constructor function that bypasses the args/kwargs unpacking
 * to allow faster creation from within C code.
 *
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *item;
    PyObject *key;
    Py_ssize_t idx;
} ItemIdxKey;

static PyTypeObject PyIUType_ItemIdxKey;

#define ItemIdxKey_CheckExact(op) (Py_TYPE(op) == &PyIUType_ItemIdxKey)

#if PY_MAJOR_VERSION == 2
#define Py_RETURN_NOTIMPLEMENTED \
    return PyErr_Format(PyExc_TypeError, "not implemented."), NULL
#endif

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    static char *kwlist[] = {"item", "idx", "key", NULL};
    ItemIdxKey *self;

    PyObject *item, *key=NULL;
    Py_ssize_t idx;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|O:ItemIdxKey", kwlist,
                                     &item, &idx, &key)) {
        return NULL;
    }

    self = (ItemIdxKey *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(item);
    Py_XINCREF(key);
    self->item = item;
    self->idx = idx;
    self->key = key;

    return (PyObject *)self;
}

static PyObject * ItemIdxKey_FromC(PyObject *item, Py_ssize_t idx,
                                   PyObject *key) {
    // STEALS REFERENCES!!!
    ItemIdxKey *self;
    // Verify inputs
    if (item == NULL) {
        PyErr_Format(PyExc_TypeError, "`item` must be given.");
        return NULL;
    }
    // Create and fill new ItemIdxKey
    self = PyObject_GC_New(ItemIdxKey, &PyIUType_ItemIdxKey);
    if (self == NULL) {
        return NULL;
    }
    self->item = item;
    self->idx = idx;
    self->key = key;
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void ItemIdxKey_dealloc(ItemIdxKey *s) {
    Py_XDECREF(s->item);
    Py_XDECREF(s->key);
    Py_TYPE(s)->tp_free((PyObject*)s);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int ItemIdxKey_traverse(ItemIdxKey *s, visitproc visit, void *arg) {
    Py_VISIT(s->item);
    Py_VISIT(s->key);
    return 0;
}

static PyObject * ItemIdxKey_richcompare(PyObject *v, PyObject *w, int op) {
    PyObject *item1, *item2;
    ItemIdxKey *l, *r;
    int ok;

    // Only allow < and > for now
    switch (op) {
        case Py_LT: break;
        case Py_GT: break;
        default: Py_RETURN_NOTIMPLEMENTED;
    }
    // only allow ItemIdxKey to be compared
    if (!ItemIdxKey_CheckExact(v) || !ItemIdxKey_CheckExact(w))
        Py_RETURN_NOTIMPLEMENTED;

    l = (ItemIdxKey *)v;
    r = (ItemIdxKey *)w;
    // Compare items if key is NULL otherwise compare keys
    if (l->key == NULL) {
        item1 = l->item;
        item2 = r->item;
    } else {
        item1 = l->key;
        item2 = r->key;
    }
    // Check if the items are the same
    ok = PyObject_RichCompareBool(item1, item2, Py_EQ);
    if (ok == -1) {
        return NULL;
    } else if (ok == 1) {
        if (l->idx < r->idx) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    } else {
        ok = PyObject_RichCompareBool(item1, item2, op);
        if (ok == -1) {
            return NULL;
        } else if (ok == 0) {
            Py_RETURN_FALSE;
        } else {
            Py_RETURN_TRUE;
        }
    }
}

/******************************************************************************
 *
 * item property
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_getitem(ItemIdxKey *self, void *closure) {
    Py_INCREF(self->item);
    return self->item;
}

int ItemIdxKey_setitem(ItemIdxKey *self,  PyObject *o, void *closure) {
    if (o == NULL) {
        PyErr_Format(PyExc_TypeError, "cannot delete `item`.");
        return -1;
    }
    Py_DECREF(self->item);
    Py_INCREF(o);
    self->item = o;
    return 0;
}

/******************************************************************************
 *
 * idx property
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_getidx(ItemIdxKey *self, void *closure) {
    return PyLong_FromSsize_t(self->idx);
}

int ItemIdxKey_setidx(ItemIdxKey *self,  PyObject *o, void *closure) {
    Py_ssize_t idx;
    if (o == NULL) {
        PyErr_Format(PyExc_TypeError, "cannot delete `idx`.");
        return -1;
    }
    idx = PyLong_AsSsize_t(o);
    if (PyErr_Occurred()) {
        return -1;
    }
    self->idx = idx;
    return 0;
}

/******************************************************************************
 *
 * key property
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_getkey(ItemIdxKey *self, void *closure) {
    if (self->key == NULL) {
        Py_RETURN_NONE;
    }
    Py_INCREF(self->key);
    return self->key;
}

int ItemIdxKey_setkey(ItemIdxKey *self,  PyObject *o, void *closure) {
    Py_XDECREF(self->key);
    Py_XINCREF(o);
    self->key = o;
    return 0;
}

/******************************************************************************
 *
 * Properties
 *
 *****************************************************************************/

static PyGetSetDef ItemIdxKey_getsetlist[] = {
    {"item", (getter)ItemIdxKey_getitem, (setter)ItemIdxKey_setitem, NULL},
    {"idx",  (getter)ItemIdxKey_getidx,  (setter)ItemIdxKey_setidx,  NULL},
    {"key",  (getter)ItemIdxKey_getkey,  (setter)ItemIdxKey_setkey,  NULL},
    {NULL}
};

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_reduce(ItemIdxKey *self) {
    if (self->key == NULL) {
        return Py_BuildValue("O(On)", Py_TYPE(self),
                             self->item, self->idx);
    } else {
        return Py_BuildValue("O(On)(O)", Py_TYPE(self),
                             self->item, self->idx, self->key);
    }
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * ItemIdxKey_setstate(ItemIdxKey *self, PyObject *state) {
    PyObject *key;
    if (!PyArg_ParseTuple(state, "O", &key)) {
        return NULL;
    }

    Py_CLEAR(self->key);
    self->key = key;
    Py_INCREF(self->key);

    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef ItemIdxKey_methods[] = {
    {"__reduce__",   (PyCFunction)ItemIdxKey_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)ItemIdxKey_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};


/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_ItemIdxKey = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.ItemIdxKey", /* tp_name */
    sizeof(ItemIdxKey),        /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)ItemIdxKey_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "ItemIdxKey object",       /* tp_doc */
    (traverseproc)ItemIdxKey_traverse, /* tp_traverse */
    0,                         /* tp_clear */
    ItemIdxKey_richcompare,    /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    ItemIdxKey_methods,        /* tp_methods */
    0,                         /* tp_members */
    ItemIdxKey_getsetlist,     /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    ItemIdxKey_new,            /* tp_new */
    PyObject_GC_Del,           /* tp_free */
};

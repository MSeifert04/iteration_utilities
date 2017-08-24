/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 *
 * Helper class that mimics a 2-tuple when compared but dynamically decides
 * which item to compare (item or key) and assumes that the idx is always
 * different.
 *
 * It also has a constructor function that bypasses the args/kwargs unpacking
 * to allow faster creation from within C code.
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *item;
    PyObject *key;
    Py_ssize_t idx;
} PyIUObject_ItemIdxKey;

static PyTypeObject PyIUType_ItemIdxKey;

#define PyIU_ItemIdxKey_Check(o) PyObject_TypeCheck(o, &PyIUType_ItemIdxKey)
#define PyIU_ItemIdxKey_CheckExact(o) (Py_TYPE(o) == &PyIUType_ItemIdxKey)

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
itemidxkey_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"item", "idx", "key", NULL};
    PyIUObject_ItemIdxKey *self;

    PyObject *item, *key=NULL;
    Py_ssize_t idx;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|O:ItemIdxKey", kwlist,
                                     &item, &idx, &key)) {
        return NULL;
    }
    if (PyIU_ItemIdxKey_Check(item)) {
        PyErr_SetString(PyExc_TypeError,
                        "`item` argument for `ItemIdxKey` must not be a "
                        "`ItemIdxKey` instance.");
        return NULL;
    }
    if (key != NULL && PyIU_ItemIdxKey_Check(key)) {
        PyErr_SetString(PyExc_TypeError,
                        "`key` argument for `ItemIdxKey` must not be a "
                        "`ItemIdxKey` instance.");
        return NULL;
    }

    self = (PyIUObject_ItemIdxKey *)type->tp_alloc(type, 0);
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

/******************************************************************************
 * New (only from C code)
 *
 * This bypasses the argument unpacking!
 *****************************************************************************/

static PyObject *
PyIU_ItemIdxKey_FromC(PyObject *item,
                      Py_ssize_t idx,
                      PyObject *key)
{
    /* STEALS REFERENCES!!! */
    PyIUObject_ItemIdxKey *self;
    /* Verifing the inputs could be done but the API isn't exported and it
       should never be NULL we can neglect this check for the sake of
       performance:
    if (item == NULL) {
        PyErr_SetString(PyExc_TypeError, "`ItemIdxKey.item` must be given.");
        return NULL;
    }
    */
    /* Create and fill new ItemIdxKey. */
    self = PyObject_GC_New(PyIUObject_ItemIdxKey, &PyIUType_ItemIdxKey);
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
 * Copy (only from C code)
 *****************************************************************************/

static PyObject *
PyIU_ItemIdxKey_Copy(PyObject *iik)
{
    PyIUObject_ItemIdxKey *n;
    PyIUObject_ItemIdxKey *o = (PyIUObject_ItemIdxKey *)iik;

    n = PyObject_GC_New(PyIUObject_ItemIdxKey, &PyIUType_ItemIdxKey);
    if (n == NULL) {
        return NULL;
    }
    Py_INCREF(o->item);
    n->item = o->item;
    n->idx = o->idx;
    Py_XINCREF(o->key);
    n->key = o->key;
    PyObject_GC_Track(n);
    return (PyObject *)n;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
itemidxkey_dealloc(PyIUObject_ItemIdxKey *self)
{
    Py_XDECREF(self->item);
    Py_XDECREF(self->key);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
itemidxkey_traverse(PyIUObject_ItemIdxKey *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->item);
    Py_VISIT(self->key);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
itemidxkey_clear(PyIUObject_ItemIdxKey *self)
{
    Py_CLEAR(self->item);
    Py_CLEAR(self->key);
    return 0;
}

/******************************************************************************
 * Representation
 *****************************************************************************/

static PyObject *
itemidxkey_repr(PyIUObject_ItemIdxKey *self)
{
    PyObject *repr;
    int ok;

    ok = Py_ReprEnter((PyObject*)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    if (self->key == NULL) {
        repr = PyUnicode_FromFormat("%s(item=%R, idx=%zd)",
                                    Py_TYPE(self)->tp_name,
                                    self->item, self->idx);
    } else {
        /* The representation of the item could modify/delete the key and then
           the representation of the key could segfault. Better to make the key
           undeletable as long as PyUnicode_FromFormat runs.
           */
        PyObject *tmpkey = self->key;
        Py_INCREF(tmpkey);
        repr = PyUnicode_FromFormat("%s(item=%R, idx=%zd, key=%R)",
                                    Py_TYPE(self)->tp_name,
                                    self->item, self->idx, tmpkey);
        Py_DECREF(tmpkey);
    }
    Py_ReprLeave((PyObject *)self);
    return repr;
}

/******************************************************************************
 * Richcompare
 *****************************************************************************/

static int
PyIU_ItemIdxKey_Compare(PyObject *v,
                        PyObject *w,
                        int op)
{
    /* Several assumptions in here:
       - That the objects are actually PyIUObject_ItemIdxKey objects.
       - That no other operator than > or < is going here.
       */
    PyObject *item1, *item2;
    PyIUObject_ItemIdxKey *l, *r;

    l = (PyIUObject_ItemIdxKey *)v;
    r = (PyIUObject_ItemIdxKey *)w;

    /* Compare items if key is NULL otherwise compare keys. */
    if (l->key == NULL) {
        item1 = l->item;
        item2 = r->item;
    } else {
        item1 = l->key;
        item2 = r->key;
    }

    /* The order to check for equality and lt makes a huge performance
       difference:
       - lots of duplicates: first eq then "op"
       - no/few duplicates: first "op" then eq
       - first compare idx and if it's smaller check le/ge otherwise lt/gt

       --> I chose eq then "op" but the other version is also avaiable as
       comment.
       */

    if (l->idx < r->idx) {
        op = (op == Py_LT) ? Py_LE : Py_GE;
    }
    return PyObject_RichCompareBool(item1, item2, op);

    /* First eq then "op" : Better if there are lots of duplicates! Worse if
                            there are few or None!
    */
    /*
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
    */

    /* First "op" then eq : Better if there are almost no duplicates! Worse if
                            there are lots!
    */
    /*
    ok = PyObject_RichCompareBool(item1, item2, op);
    if (ok == 1) {
        Py_RETURN_TRUE;
    } else if (ok == 0) {
        ok = PyObject_RichCompareBool(item1, item2, Py_EQ);
        if (ok == 1) {
            if (l->idx < r->idx) {
                Py_RETURN_TRUE;
            } else {
                Py_RETURN_FALSE;
            }
        } else if (ok == 0) {
            Py_RETURN_FALSE;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    */

    /* First compare idx and then compare the other elements.
    */
    /*
    if (l->idx < r->idx) {
        op = (op == Py_LT) ? Py_LE : Py_GE;
    }
    ok = PyObject_RichCompareBool(item1, item2, op);
    if (ok == 1) {
        Py_RETURN_TRUE;
    } else if (ok == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
    */
}


static PyObject *
itemidxkey_richcompare(PyObject *v,
                       PyObject *w,
                       int op)
{
    int ok;

    /* Only allow < and > for now */
    switch (op) {
        case Py_LT: break;
        case Py_GT: break;
        default: Py_RETURN_NOTIMPLEMENTED;
    }
    /* only allow ItemIdxKey to be compared. */
    if (!PyIU_ItemIdxKey_Check(v) || !PyIU_ItemIdxKey_Check(w))
        Py_RETURN_NOTIMPLEMENTED;

    ok = PyIU_ItemIdxKey_Compare(v, w, op);
    if (ok == 1) {
        Py_RETURN_TRUE;
    } else if (ok == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * item Property
 *****************************************************************************/

static PyObject *
itemidxkey_getitem(PyIUObject_ItemIdxKey *self,
                   void *closure)
{
    Py_INCREF(self->item);
    return self->item;
}

static int
itemidxkey_setitem(PyIUObject_ItemIdxKey *self,
                   PyObject *o,
                   void *closure)
{
    if (o == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot delete `item` attribute of `ItemIdxKey`.");
        return -1;
    } else if (PyIU_ItemIdxKey_Check(o)) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot use `ItemIdxKey` instance as `item` of "
                        "`ItemIdxKey`.");
        return -1;
    }
    Py_DECREF(self->item);
    Py_INCREF(o);
    self->item = o;
    return 0;
}

/******************************************************************************
 * idx Property
 *****************************************************************************/

static PyObject *
itemidxkey_getidx(PyIUObject_ItemIdxKey *self,
                  void *closure)
{
    #if PY_MAJOR_VERSION == 2
        return PyInt_FromSsize_t(self->idx);
    #else
        return PyLong_FromSsize_t(self->idx);
    #endif
}

static int
itemidxkey_setidx(PyIUObject_ItemIdxKey *self,
                  PyObject *o,
                  void *closure)
{
    Py_ssize_t idx;
    if (o == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot delete `idx` attribute of `ItemIdxKey`.");
        return -1;
    }
    #if PY_MAJOR_VERSION == 2
    if (PyInt_Check(o)) {
        idx = PyInt_AsSsize_t(o);
    } else
    #endif
    if (PyLong_Check(o)) {
        idx = PyLong_AsSsize_t(o);
    } else {
        PyErr_SetString(PyExc_TypeError,
                        "an integer is required as `idx` attribute of "
                        "`ItemIdxKey`.");
        return -1;
    }

    if (PyErr_Occurred()) {
        return -1;
    }

    self->idx = idx;
    return 0;
}

/******************************************************************************
 * key Property
 *****************************************************************************/

static PyObject *
itemidxkey_getkey(PyIUObject_ItemIdxKey *self,
                  void *closure)
{
    if (self->key == NULL) {
        PyErr_SetString(PyExc_AttributeError,
                        "the `key` attribute of `ItemIdxKey` instance is not "
                        "set.");
        return NULL;
    }
    Py_INCREF(self->key);
    return self->key;
}

static int
itemidxkey_setkey(PyIUObject_ItemIdxKey *self,
                  PyObject *o,
                  void *closure)
{
    if (o != NULL && PyIU_ItemIdxKey_Check(o)) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot use `ItemIdxKey` instance as `key` attribute "
                        "of `ItemIdxKey`.");
        return -1;
    }
    /* Cannot delete an non-existing attribute... */
    if (o == NULL && self->key == NULL) {
        PyErr_SetString(PyExc_AttributeError,
                        "the `key` attribute of `ItemIdxKey` instance is not "
                        "set and cannot be deleted.");
        return -1;
    }
    Py_XDECREF(self->key);
    Py_XINCREF(o);
    self->key = o;
    return 0;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
itemidxkey_reduce(PyIUObject_ItemIdxKey *self)
{
    if (self->key == NULL) {
        return Py_BuildValue("O(On)", Py_TYPE(self),
                             self->item, self->idx);
    } else {
        return Py_BuildValue("O(OnO)", Py_TYPE(self),
                             self->item, self->idx, self->key);
    }
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef itemidxkey_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)itemidxkey_reduce,                    /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

static PyGetSetDef itemidxkey_getsetlist[] = {

    {"item",                                            /* name */
     (getter)itemidxkey_getitem,                        /* get */
     (setter)itemidxkey_setitem,                        /* set */
     itemidxkey_prop_item_doc,                          /* doc */
     (void *)NULL                                       /* closure */
     },

    {"idx",                                             /* name */
     (getter)itemidxkey_getidx,                         /* get */
     (setter)itemidxkey_setidx,                         /* set */
     itemidxkey_prop_idx_doc,                           /* doc */
     (void *)NULL                                       /* closure */
     },

    {"key",                                             /* name */
     (getter)itemidxkey_getkey,                         /* get */
     (setter)itemidxkey_setkey,                         /* set */
     itemidxkey_prop_key_doc,                           /* doc */
     (void *)NULL                                       /* closure */
     },

    {NULL}                                              /* sentinel */
};

static PyTypeObject PyIUType_ItemIdxKey = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.ItemIdxKey",     /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_ItemIdxKey),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)itemidxkey_dealloc,                     /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)itemidxkey_repr,                          /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash  */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)0,                                    /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)itemidxkey_doc,                       /* tp_doc */
    (traverseproc)itemidxkey_traverse,                  /* tp_traverse */
    (inquiry)itemidxkey_clear,                          /* tp_clear */
    (richcmpfunc)itemidxkey_richcompare,                /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    itemidxkey_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    itemidxkey_getsetlist,                              /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)itemidxkey_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};

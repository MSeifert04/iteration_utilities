/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "itemidxkey.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    itemidxkey_prop_item_doc,
    "(any type) The `item` to sort.");

PyDoc_STRVAR(
    itemidxkey_prop_idx_doc,
    "(:py:class:`int`) The original position of the `item`.");

PyDoc_STRVAR(
    itemidxkey_prop_key_doc,
    "(any type) The result of a key function applied to the `item`.");

PyDoc_STRVAR(
    itemidxkey_doc,
    "ItemIdxKey(item, idx, /, key)\n"
    "--\n\n"
    "Helper class that makes it easier and faster to compare two values for\n"
    "*stable* sorting algorithms supporting key functions.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "item : any type\n"
    "    The original `item`.\n"
    "\n"
    "idx : :py:class:`int`\n"
    "    The position (index) of the `item`.\n"
    "\n"
    "key : any type, optional\n"
    "    If given (even as ``None``) this should be the `item` processed by the \n"
    "    `key` function. If it is set then comparisons will compare the `key` \n"
    "    instead of the `item`.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "Comparisons involving :py:class:`~iteration_utilities.ItemIdxKey` have some \n"
    "limitations:\n"
    "\n"
    "- Both have to be :py:class:`~iteration_utilities.ItemIdxKey` instances.\n"
    "- If the first operand has no :py:attr:`.key` then the :py:attr:`.item` are \n"
    "  compared.\n"
    "- The :py:attr:`.idx` must be different.\n"
    "- only :py:meth:`< <.__lt__>` and :py:meth:`> <.__gt__>` are supported!\n"
    "\n"
    "The implementation is roughly like:\n"
    "\n"
    ".. code::\n"
    "\n"
    "   _notgiven = object()\n"
    "   \n"
    "   class ItemIdxKey:\n"
    "       def __init__(self, item, idx, key=_notgiven):\n"
    "           self.item = item\n"
    "           self.idx = idx\n"
    "           self.key = key\n"
    "   \n"
    "       def __lt__(self, other):\n"
    "           if type(other) != ItemIdxKey:\n"
    "               raise TypeError()\n"
    "           if self.key is _notgiven:\n"
    "               item1, item2 = self.item, other.item\n"
    "           else:\n"
    "               item1, item2 = self.key, other.key\n"
    "           if self.idx < other.idx:\n"
    "               return item1 <= item2\n"
    "           else:\n"
    "               return item1 < item2\n"
    "   \n"
    "       def __gt__(self, other):\n"
    "           if type(other) != ItemIdxKey:\n"
    "               raise TypeError()\n"
    "           if self.key is _notgiven:\n"
    "               item1, item2 = self.item, other.item\n"
    "           else:\n"
    "               item1, item2 = self.key, other.key\n"
    "           if self.idx < other.idx:\n"
    "               return item1 >= item2\n"
    "           else:\n"
    "               return item1 > item2\n"
    "\n"
    ".. note::\n"
    "   The actual C makes the initialization and comparisons several times faster\n"
    "   than the above illustrated Python class! But it's only slightly faster\n"
    "   than comparing :py:class:`tuple` or :py:class:`list`. If you do not plan \n"
    "   to support `reverse` or `key` then there is no need to use this class!\n"
    "\n"
    ".. warning::\n"
    "   You should **never** insert a :py:class:`~iteration_utilities.ItemIdxKey` \n"
    "   instance as :py:attr:`.item` or :py:attr:`.key` in another\n"
    "   :py:class:`~iteration_utilities.ItemIdxKey` instance. This would yield \n"
    "   wrong results and breaks your computer! (the latter might not be true.)\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Stability is one of the distinct features of sorting algorithms. This class\n"
    "aids in supporting those algorithms which allow `reverse` and `key`.\n"
    "This means that comparisons require absolute lesser (or greater if `reverse`)\n"
    "if the :py:attr:`.idx` is bigger but only require lesser or equal (or greater or equal)\n"
    "if the :py:attr:`.idx` is smaller. This class implements exactly these conditions::\n"
    "\n"
    "    >>> # Use < for normal sorting.\n"
    "    >>> ItemIdxKey(10, 2) < ItemIdxKey(10, 3)\n"
    "    True\n"
    "    >>> # and > for reverse sorting.\n"
    "    >>> ItemIdxKey(10, 2) > ItemIdxKey(10, 3)\n"
    "    True\n"
    "\n"
    "The result may seem surprising but if the :py:attr:`.item` (or :py:attr:`.key`) is equal then\n"
    "in either normal or `reverse` sorting the one with the smaller :py:attr:`.idx` should\n"
    "come first! If the :py:attr:`.item` (or :py:attr:`.key`) differ they take precedence.\n"
    "\n"
    "    >>> ItemIdxKey(10, 2) < ItemIdxKey(11, 3)\n"
    "    True\n"
    "    >>> ItemIdxKey(10, 2) > ItemIdxKey(11, 3)\n"
    "    False\n"
    "\n"
    "But it compares the :py:attr:`.key` instead of the :py:attr:`.item` if it's given::\n"
    "\n"
    "    >>> ItemIdxKey(0, 2, 20) < ItemIdxKey(10, 3, 19)\n"
    "    False\n"
    "    >>> ItemIdxKey(0, 2, 20) > ItemIdxKey(10, 3, 19)\n"
    "    True\n"
    "\n"
    "This allows to sort based on :py:attr:`.item` or :py:attr:`.key` but always \n"
    "to access the :py:attr:`.item` for the value that should be sorted.\n");

/******************************************************************************
 *
 * Helper class that mimics a 2-tuple when compared but dynamically decides
 * which item to compare (item or key) and assumes that the idx is always
 * different.
 *
 * It also has a constructor function that bypasses the args/kwargs unpacking
 * to allow faster creation from within C code.
 *****************************************************************************/

/******************************************************************************
 * New (only from C code)
 *
 * This bypasses the argument unpacking!
 *****************************************************************************/

PyObject *
PyIU_ItemIdxKey_FromC(PyObject *item, Py_ssize_t idx, PyObject *key) {
    /* STEALS REFERENCES!!! */
    assert(item != NULL);
    assert(!PyIU_ItemIdxKey_Check(item));
    assert(key == NULL || !PyIU_ItemIdxKey_Check(key));

    PyIUObject_ItemIdxKey *self;
    /* Create and fill new ItemIdxKey. */
    self = PyObject_GC_New(PyIUObject_ItemIdxKey, &PyIUType_ItemIdxKey);
    if (self == NULL) {
        // So that the function always steals references even if allocation failed.
        Py_DECREF(item);
        Py_XDECREF(key);
        return NULL;
    }
    self->item = item;
    self->idx = idx;
    self->key = key;
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

PyObject *
PyIU_ItemIdxKey_Copy(PyObject *iik) {
    assert(iik != NULL && PyIU_IsTypeExact(iik, &PyIUType_ItemIdxKey));

    PyIUObject_ItemIdxKey *o = (PyIUObject_ItemIdxKey *)iik;
    Py_INCREF(o->item);
    Py_XINCREF(o->key);
    return PyIU_ItemIdxKey_FromC(o->item, o->idx, o->key);
}

static PyObject *
itemidxkey_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"item", "idx", "key", NULL};
    PyIUObject_ItemIdxKey *self;
    PyObject *item;
    PyObject *key = NULL;
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

static void
itemidxkey_dealloc(PyIUObject_ItemIdxKey *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->item);
    Py_XDECREF(self->key);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int
itemidxkey_traverse(PyIUObject_ItemIdxKey *self, visitproc visit, void *arg) {
    Py_VISIT(self->item);
    Py_VISIT(self->key);
    return 0;
}

static int
itemidxkey_clear(PyIUObject_ItemIdxKey *self) {
    Py_CLEAR(self->item);
    Py_CLEAR(self->key);
    return 0;
}

static PyObject *
itemidxkey_repr(PyIUObject_ItemIdxKey *self) {
    PyObject *repr;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
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

int PyIU_ItemIdxKey_Compare(PyObject *v, PyObject *w, int op) {
    assert(v != NULL && PyIU_ItemIdxKey_Check(v));
    assert(w != NULL && PyIU_ItemIdxKey_Check(w));
    assert(op == Py_GT || op == Py_LT);

    PyObject *item1;
    PyObject *item2;
    PyIUObject_ItemIdxKey *l;
    PyIUObject_ItemIdxKey *r;

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

       --> I chose eq then "op"
       */

    if (l->idx < r->idx) {
        op = (op == Py_LT) ? Py_LE : Py_GE;
    }
    return PyObject_RichCompareBool(item1, item2, op);
}

static PyObject *
itemidxkey_richcompare(PyObject *v, PyObject *w, int op) {
    int ok;

    /* Only allow < and > for now */
    switch (op) {
        case Py_LT:
        case Py_GT:
            break;
        default:
            Py_RETURN_NOTIMPLEMENTED;
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

static PyObject *
itemidxkey_getitem(PyIUObject_ItemIdxKey *self, void *Py_UNUSED(closure)) {
    Py_INCREF(self->item);
    return self->item;
}

static int
itemidxkey_setitem(PyIUObject_ItemIdxKey *self, PyObject *o, void *Py_UNUSED(closure)) {
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
    Py_INCREF(o);
    Py_SETREF(self->item, o);
    return 0;
}

static PyObject *
itemidxkey_getidx(PyIUObject_ItemIdxKey *self, void *Py_UNUSED(closure)) {
    return PyLong_FromSsize_t(self->idx);
}

static int
itemidxkey_setidx(PyIUObject_ItemIdxKey *self, PyObject *o, void *Py_UNUSED(closure)) {
    Py_ssize_t idx;
    if (o == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot delete `idx` attribute of `ItemIdxKey`.");
        return -1;
    }
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

static PyObject *
itemidxkey_getkey(PyIUObject_ItemIdxKey *self, void *Py_UNUSED(closure)) {
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
itemidxkey_setkey(PyIUObject_ItemIdxKey *self, PyObject *o, void *Py_UNUSED(closure)) {
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
    Py_XINCREF(o);
    Py_XSETREF(self->key, o);
    return 0;
}

static PyObject *
itemidxkey_reduce(PyIUObject_ItemIdxKey *self, PyObject *Py_UNUSED(args)) {
    if (self->key == NULL) {
        return Py_BuildValue("O(On)", Py_TYPE(self),
                             self->item, self->idx);
    } else {
        return Py_BuildValue("O(OnO)", Py_TYPE(self),
                             self->item, self->idx, self->key);
    }
}

static PyMethodDef itemidxkey_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)itemidxkey_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyGetSetDef itemidxkey_getsetlist[] = {
    {
        "item",                     /* name */
        (getter)itemidxkey_getitem, /* get */
        (setter)itemidxkey_setitem, /* set */
        itemidxkey_prop_item_doc,   /* doc */
        (void *)NULL                /* closure */
    },
    {
        "idx",                     /* name */
        (getter)itemidxkey_getidx, /* get */
        (setter)itemidxkey_setidx, /* set */
        itemidxkey_prop_idx_doc,   /* doc */
        (void *)NULL               /* closure */
    },
    {
        "key",                     /* name */
        (getter)itemidxkey_getkey, /* get */
        (setter)itemidxkey_setkey, /* set */
        itemidxkey_prop_key_doc,   /* doc */
        (void *)NULL               /* closure */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_ItemIdxKey = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.ItemIdxKey", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_ItemIdxKey),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)itemidxkey_dealloc, /* tp_dealloc */
    (printfunc)0,                   /* tp_print */
    (getattrfunc)0,                 /* tp_getattr */
    (setattrfunc)0,                 /* tp_setattr */
    0,                              /* tp_reserved */
    (reprfunc)itemidxkey_repr,      /* tp_repr */
    (PyNumberMethods *)0,           /* tp_as_number */
    (PySequenceMethods *)0,         /* tp_as_sequence */
    (PyMappingMethods *)0,          /* tp_as_mapping */
    (hashfunc)0,                    /* tp_hash  */
    (ternaryfunc)0,                 /* tp_call */
    (reprfunc)0,                    /* tp_str */
    (getattrofunc)0,                /* tp_getattro */
    (setattrofunc)0,                /* tp_setattro */
    (PyBufferProcs *)0,             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,             /* tp_flags */
    (const char *)itemidxkey_doc,        /* tp_doc */
    (traverseproc)itemidxkey_traverse,   /* tp_traverse */
    (inquiry)itemidxkey_clear,           /* tp_clear */
    (richcmpfunc)itemidxkey_richcompare, /* tp_richcompare */
    (Py_ssize_t)0,                       /* tp_weaklistoffset */
    (getiterfunc)0,                      /* tp_iter */
    (iternextfunc)0,                     /* tp_iternext */
    itemidxkey_methods,                  /* tp_methods */
    0,                                   /* tp_members */
    itemidxkey_getsetlist,               /* tp_getset */
    0,                                   /* tp_base */
    0,                                   /* tp_dict */
    (descrgetfunc)0,                     /* tp_descr_get */
    (descrsetfunc)0,                     /* tp_descr_set */
    (Py_ssize_t)0,                       /* tp_dictoffset */
    (initproc)0,                         /* tp_init */
    (allocfunc)0,                        /* tp_alloc */
    (newfunc)itemidxkey_new,             /* tp_new */
    (freefunc)PyObject_GC_Del,           /* tp_free */
};

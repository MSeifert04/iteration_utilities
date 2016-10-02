typedef struct {
    PyObject_HEAD
    PyObject *it;
    Py_ssize_t times;
    PyObject *result;
} recipes_successive_object;


static PyObject *
recipes_successive_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "times", NULL};
    PyObject *iterable;
    Py_ssize_t times = 2;
    PyObject *it;
    PyObject *result = NULL;
    recipes_successive_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|n:successive", kwargs,
                                     &iterable, &times)) {
        return NULL;
    }

    if (times <= 0) {
        PyErr_Format(PyExc_ValueError,
                     "times must be greater than 0.");
        return NULL;
    }

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    /* create recipes_successive_object structure */
    lz = (recipes_successive_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    lz->it = it;
    lz->times = times;
    lz->result = result;

    return (PyObject *)lz;
}


static void
recipes_successive_dealloc(recipes_successive_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->result);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_successive_traverse(recipes_successive_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->it);
    Py_VISIT(lz->result);
    return 0;
}


static PyObject *
recipes_successive_next(recipes_successive_object *lz)
{
    Py_ssize_t times = lz->times;
    Py_ssize_t i;

    PyObject *result = lz->result;
    PyObject *it = lz->it;

    PyObject *newresult;
    PyObject *item, *olditem, *temp=NULL;

    // First call needs to create a tuple for the result.
    if (result == NULL) {
        result = PyTuple_New(times);
        if (result == NULL) {
            return NULL;
        }

        for (i=0; i < times; i++) {
            item = (*Py_TYPE(it)->tp_iternext)(it);
            if (item == NULL) {
                Py_DECREF(result);
                return NULL;
            }
            PyTuple_SET_ITEM(result, i, item);
        }
        Py_INCREF(result);
        lz->result = result;
        return result;
    }

    // After the first element we can use the normal procedure.
    item = PyIter_Next(it);
    if (item == NULL) {
        return NULL;
    }

    // Recycle old tuple or create a new one.
    if (Py_REFCNT(result) == 1) {

        // Shift all earlier items one index to the left.
        for (i=1 ; i < times ; i++) {
            olditem = PyTuple_GET_ITEM(result, i);
            Py_INCREF(olditem);
            if (i == 1) {
                // Temporarly get the first item because it's going to be kicked.
                temp = PyTuple_GET_ITEM(result, 0);
                PyTuple_SET_ITEM(result, i-1, olditem);
            } else {
                PyTuple_SET_ITEM(result, i-1, olditem);
            }
        }
        // Insert the new item
        PyTuple_SET_ITEM(result, times-1, item);
        Py_INCREF(result);
        Py_XDECREF(temp);

        return result;

    } else {
        newresult = PyTuple_New(times);
        if (newresult == NULL) {
            Py_DECREF(item);
            return NULL;
        }

        // Shift all earlier items one index to the left.
        for (i=1 ; i < times ; i++) {
            olditem = PyTuple_GET_ITEM(result, i);
            Py_INCREF(olditem);
            PyTuple_SET_ITEM(newresult, i-1, olditem);
        }
        Py_DECREF(result);

        // Insert the new item
        PyTuple_SET_ITEM(newresult, times-1, item);

        Py_INCREF(newresult);
        lz->result = newresult;
        return newresult;
    }
}


static PyObject *
recipes_successive_reduce(recipes_successive_object *lz)
{
    return Py_BuildValue("O(On)(O)", Py_TYPE(lz),
                         lz->it, lz->times,
                         lz->result ? lz->result : Py_None);
}

static PyObject *
recipes_successive_setstate(recipes_successive_object *lz, PyObject *state)
{
    PyObject *result;
    if (!PyArg_ParseTuple(state, "O", &result)) {
        return NULL;
    }

    Py_CLEAR(lz->result);

    if (result == Py_None) {
        lz->result = NULL;
    } else {
        Py_INCREF(result);
        lz->result = result;
    }

    Py_RETURN_NONE;
}


static PyMethodDef recipes_successive_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_successive_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_successive_setstate,
     METH_O,
     ""},

    {NULL,           NULL}           /* sentinel */
};


PyDoc_STRVAR(recipes_successive_doc,
"successive(iterable, times)\n\
\n\
Like the recipe for pairwise but allows to get an arbitary number\n\
of successive elements.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Get the successive elements from this `iterable`.\n\
\n\
times : integer, optional\n\
    The number of successive elements.\n\
    Default is ``2``.\n\
\n\
Returns\n\
-------\n\
successive_elements : generator\n\
    The successive elements as generator. Each element of the generator\n\
    is a tuple containing `times` successive elements.\n\
\n\
Examples\n\
--------\n\
Each item of the `iterable` is returned as ``tuple`` with `times` successive\n\
items::\n\
\n\
    >>> from iteration_utilities import successive\n\
    >>> list(successive(range(5)))\n\
    [(0, 1), (1, 2), (2, 3), (3, 4)]\n\
\n\
Varying the `times` can give you also 3 successive elements::\n\
\n\
    >>> list(successive(range(5), times=3))\n\
    [(0, 1, 2), (1, 2, 3), (2, 3, 4)]\n\
    >>> list(successive('Hello!', times=2))\n\
    [('H', 'e'), ('e', 'l'), ('l', 'l'), ('l', 'o'), ('o', '!')]\n\
");

PyTypeObject recipes_successive_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.successive",   /* tp_name */
    sizeof(recipes_successive_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_successive_dealloc, /* tp_dealloc */
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
    recipes_successive_doc,             /* tp_doc */
    (traverseproc)recipes_successive_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_successive_next, /* tp_iternext */
    recipes_successive_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    recipes_successive_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
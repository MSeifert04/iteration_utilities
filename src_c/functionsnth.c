typedef struct {
    PyObject_HEAD
    Py_ssize_t index;
} functions_nth_object;

static PyTypeObject functions_nth_type;


static PyObject *
functions_nth_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{

    Py_ssize_t index;
    functions_nth_object *lz;

    static char *kwargs[] = {"n", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "n:nth", kwargs,
                                     &index)) {
        return NULL;
    }

    lz = (functions_nth_object *)type->tp_alloc(type, 0);
    if (lz == NULL)
        return NULL;

    lz->index = index;
    return (PyObject *)lz;
}


static void
functions_nth_dealloc(functions_nth_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_TYPE(lz)->tp_free(lz);
}


static int
functions_nth_traverse(functions_nth_object *lz, visitproc visit, void *arg)
{
    return 0;
}


static PyObject *
functions_nth_call(functions_nth_object *lz, PyObject *args, PyObject *kwds)
{
    Py_ssize_t n = lz->index;
    PyObject *iterable, *defaultitem=NULL, *func=NULL;
    PyObject *(*iternext)(PyObject *);
    int truthy=1, retpred=0, retidx=0;

    PyObject *iterator, *item=NULL, *last=NULL, *val=NULL;
    Py_ssize_t i, idx=-1;
    int ok;

    static char *kwlist[] = {"iterable", "default", "pred", "truthy",
                             "retpred", "retidx", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOiii:nth", kwlist,
                                     &iterable, &defaultitem, &func,
                                     &truthy, &retpred, &retidx)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    if (retpred && retidx) {
        PyErr_Format(PyExc_ValueError, "can only specify `retpred` or `retidx`.");
        return NULL;
    }

    iternext = *Py_TYPE(iterator)->tp_iternext;

    for (i=0 ; i<=n || n < 0; ) {
        item = iternext(iterator);
        if (item == NULL) {
            // Keep the last one in case we looked for the last one.
            if (n >= 0) {
                Py_XDECREF(last);
                last = NULL;
            }
            break;
        }
        if (retidx)
            idx++;
        // Sequence contains an element and func is None: return it.
        if (func == NULL) {
            if (last != NULL) {
                Py_DECREF(last);
            }
            last = item;
            i++;
            continue;

        } else if (func == Py_None || func == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);

        } else {
            val = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(item);
                Py_XDECREF(last);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
        }

        if (ok == truthy) {
            if (retpred) {
                Py_DECREF(item);
                if (val == NULL) {
                    val = PyBool_FromLong(ok);
                }
                if (last != NULL) {
                    Py_DECREF(last);
                }
                last = val;

            } else {
                Py_XDECREF(val);
                if (last != NULL) {
                    Py_DECREF(last);
                }
                last = item;
            }
            i++;

        } else if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            Py_XDECREF(val);
            return NULL;

        } else {
            Py_DECREF(item);
            Py_XDECREF(val);
        }
    }

    Py_DECREF(iterator);

    helper_ExceptionClearStopIter();

    if (last != NULL) {
        if (retidx) {
            Py_DECREF(last);
            return PyLong_FromSsize_t(idx);
        }
        return last;
    }

    Py_XDECREF(last);

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_IndexError, "not enough values.");
        return NULL;
    } else {
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


static PyObject *
functions_nth_reduce(functions_nth_object *lz, PyObject *unused)
{
    return Py_BuildValue("O(n)", Py_TYPE(lz),
                         lz->index);
}

static PyMethodDef functions_nth_methods[] = {
    {"__reduce__",
     (PyCFunction)functions_nth_reduce,
     METH_NOARGS,
     ""},

    {NULL,              NULL}
};


PyDoc_STRVAR(functions_nth_doc,
"nth(x)\n\
\n\
");


static PyTypeObject functions_nth_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.nth_cls",      /* tp_name */
    sizeof(functions_nth_object),       /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)functions_nth_dealloc,  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)functions_nth_call,    /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    functions_nth_doc,                  /* tp_doc */
    (traverseproc)functions_nth_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    functions_nth_methods,              /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    functions_nth_new,                  /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};

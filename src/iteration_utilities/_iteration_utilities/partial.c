/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "partial.h"
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "docs_sizeof.h"
#include "placeholder.h"
#include "helper.h"
#include <structmember.h>

PyDoc_STRVAR(partial_prop_func_doc,
    "(callable) Function object to use in future partial calls (readonly).");
PyDoc_STRVAR(partial_prop_args_doc,
    "(:py:class:`tuple`) arguments for future partial calls (readonly).");
PyDoc_STRVAR(partial_prop_keywords_doc,
    "(:py:class:`dict`) keyword arguments for future partial calls (readonly).");
PyDoc_STRVAR(partial_prop_nplaceholders_doc,
    "(:py:class:`int`) Number of placeholders in the args (readonly).");
PyDoc_STRVAR(partial_prop___dict___doc,
    "");

PyDoc_STRVAR(partial_doc,
    "partial(func, *args, **kwargs)\n"
    "--\n\n"
    "Like :py:func:`functools.partial` but supporting placeholders.\n"
    "\n"
    ".. versionadded:: 0.4.0\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "\n"
    "func : callable\n"
    "    The function to partially wrap.\n"
    "\n"
    "args : any type\n"
    "    The positional arguments for `func`.\n"
    "    \n"
    "    .. note::\n"
    "       Using :py:attr:`.partial._` as one or multiple positional arguments \n"
    "       will be interpreted as placeholder that need to be filled when the \n"
    "       :py:class:`~iteration_utilities.partial` instance is called.\n"
    "\n"
    "kwargs : any type\n"
    "    The keyword arguments for `func`.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "\n"
    "partial : callable\n"
    "    The `func` where the given positional arguments are fixed (or represented\n"
    "    as placeholders) and with optional keyword arguments.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "While placeholders can be used for the :py:attr:`args` they can't be used \n"
    "for the :py:attr:`keywords`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "The :py:class:`iteration_utilities.partial` can be used as slightly slower\n"
    "drop-in replacement for :py:func:`functools.partial`. However it offers the\n"
    "possibility to pass in placeholders as positional arguments. This can be\n"
    "especially useful if a function does not allow keyword arguments::\n"
    "\n"
    "    >>> from iteration_utilities import partial\n"
    "    >>> isint = partial(isinstance, partial._, int)\n"
    "    >>> isint(10)\n"
    "    True\n"
    "    >>> isint(11.11)\n"
    "    False\n"
    "\n"
    "In this case the `isint` function is equivalent but faster than\n"
    "``lambda x: isinstance(x, int)``.\n"
    "The :py:attr:`.partial._` attribute or the \n"
    ":py:const:`~iteration_utilities.Placeholder`  or instances of \n"
    ":py:func:`~iteration_utilities.PlaceholderType` can be used as placeholders \n"
    "for the positional arguments.\n"
    "\n"
    "For example most iterators in :py:mod:`iteration_utilities` take the `iterable` \n"
    "as the first argument so other arguments can be easily added::\n"
    "\n"
    "    >>> from iteration_utilities import accumulate, Placeholder\n"
    "    >>> from operator import mul\n"
    "    >>> cumprod = partial(accumulate, Placeholder, mul)\n"
    "    >>> list(cumprod([1,2,3,4,5]))\n"
    "    [1, 2, 6, 24, 120]\n"
);

/******************************************************************************
 * Helper to get the amount and positions of Placeholders in a tuple.
 *****************************************************************************/

static Py_ssize_t
PyIUPlaceholder_NumInTuple(PyObject *tup)
{
    Py_ssize_t cnts = 0;
    Py_ssize_t i;

    /* Find the placeholders (if any) in the tuple. */
    for ( i=0 ; i < PyTuple_GET_SIZE(tup) ; i++ ) {
        if (PyTuple_GET_ITEM(tup, i) == PYIU_Placeholder) {
            cnts++;
        }
    }

    return cnts;
}

static Py_ssize_t *
PyIUPlaceholder_PosInTuple(PyObject *tup,
                           Py_ssize_t cnts)
{
    Py_ssize_t *pos = PyMem_Malloc((size_t)cnts * sizeof(Py_ssize_t));
    Py_ssize_t j = 0;
    Py_ssize_t i;

    if (pos == NULL) {
        PyErr_SetString(PyExc_MemoryError,
                        "Memory Error when trying to allocate array for "
                        "`partial`.");
        goto Fail;
    }

    /* Find the placeholders (if any) in the tuple. */
    for ( i=0 ; i < PyTuple_GET_SIZE(tup) ; i++ ) {
        if (PyTuple_GET_ITEM(tup, i) == PYIU_Placeholder) {
            pos[j] = i;
            j++;
        }
    }

    if (j != cnts) {
        PyErr_SetString(PyExc_TypeError,
                        "Something went wrong... totally wrong!");
        goto Fail;
    }

    return pos;

Fail:
    PyMem_Free(pos);
    return NULL;

}

/******************************************************************************
 * Parts are taken from the CPython package (PSF licensed).
 *****************************************************************************/

/******************************************************************************
 * Dealloc
 *****************************************************************************/

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
    if (self->posph != NULL) {
        PyMem_Free(self->posph);
    }
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
partial_traverse(PyIUObject_Partial *self,
                 visitproc visit,
                 void *arg)
{
    Py_VISIT(self->fn);
    Py_VISIT(self->args);
    Py_VISIT(self->kw);
    Py_VISIT(self->dict);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
partial_clear(PyIUObject_Partial *self)
{
    Py_CLEAR(self->fn);
    Py_CLEAR(self->args);
    Py_CLEAR(self->kw);
    Py_CLEAR(self->dict);
    /* TODO: Is it necessary to clear the self->posph array here? Probably not
             because it doesn't contain PyObjects ...
       */
    return 0;
}

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
partial_new(PyTypeObject *type,
            PyObject *args,
            PyObject *kw)
{
    PyObject *func;
    PyObject *nargs;
    PyObject *pargs = NULL;
    PyObject *pkw = NULL;
    PyIUObject_Partial *self = NULL;
    Py_ssize_t startslice = 1;

    if (PyTuple_GET_SIZE(args) < 1) {
        PyErr_SetString(PyExc_TypeError,
                        "`partial` takes at least one argument");
        goto Fail;
    }

    /* create PyIUObject_Partial structure */
    self = (PyIUObject_Partial *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }

    func = PyTuple_GET_ITEM(args, 0);
    /* Unwrap the function; if it's another partial and we're not in a subclass
       and (that's important) there is no custom attribute set
       (__dict__ = NULL). That means even if the dict was only accessed but
       empty!
       */
    if (Py_TYPE(func) == &PyIUType_Partial &&
            type == &PyIUType_Partial &&
            ((PyIUObject_Partial *)func)->dict == NULL) {

        Py_ssize_t tuplesize = PyTuple_GET_SIZE(args) - 1;
        PyIUObject_Partial *part = (PyIUObject_Partial *)func;

        if (part->numph && tuplesize) {
            /* Creating a partial from another partial which had placeholders
               needs to be specially treated. At least if there are positional
               keywords given these will replace the placeholders!
               */
            Py_ssize_t i, stop;

            pargs = PyIU_TupleCopy(part->args);
            if (pargs == NULL) {
                return NULL;
            }
            /* Only replace min(part->numph, tuplesize) placeholders, otherwise
               this will make out of bounds memory accesses (besides doing
               something undefined).
               */
            stop = part->numph > tuplesize ? tuplesize : part->numph;
            for ( i=0 ; i < stop ; i++ ) {
                PyObject *tmp = PyTuple_GET_ITEM(args, i+1);
                PyObject *ph = PyTuple_GET_ITEM(pargs, part->posph[i]);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(pargs, part->posph[i], tmp);
                Py_DECREF(ph);
            }
            /* Just alter the startslice so the arguments will be sliced
               correctly later. It is also a good indicator if the pargs need
               to be decremented later. */
            startslice = startslice + stop;
        } else {
            pargs = part->args;
        }
        pkw = part->kw;
        func = part->fn;
    }

    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError,
                        "the first argument for `partial` must be callable");
        goto Fail;
    }
    self->posph = NULL;

    self->fn = func;
    Py_INCREF(func);

    nargs = PyTuple_GetSlice(args, startslice, PY_SSIZE_T_MAX);
    if (nargs == NULL) {
        goto Fail;
    }

    if (pargs == NULL || PyTuple_GET_SIZE(pargs) == 0) {
        /* Save the arguments. */
        self->args = nargs;
        Py_INCREF(nargs);
    } else if (PyTuple_GET_SIZE(nargs) == 0) {
        self->args = pargs;
        Py_INCREF(pargs);
    } else {
        self->args = PySequence_Concat(pargs, nargs);
        if (self->args == NULL) {
            Py_DECREF(nargs);
            goto Fail;
        }
    }
    /* Check how many placeholders exist and at which positions. */
    self->numph = PyIUPlaceholder_NumInTuple(self->args);
    if (self->numph) {
        self->posph = PyIUPlaceholder_PosInTuple(self->args, self->numph);
        if (self->posph == NULL) {
            goto Fail;
        }
    }
    Py_DECREF(nargs);
    /* If we already exchanged placeholders we already got a reference to
       pargs so we need to decrement them once. */
    if (startslice != 1) {
        Py_DECREF(pargs);
        startslice = 1;  /* So the "Fail" won't decrement them again. */
    }

    if (pkw == NULL || PyDict_Size(pkw) == 0) {
        if (kw == NULL) {
            self->kw = PyDict_New();
        } else if (PYIU_CPYTHON && (Py_REFCNT(kw) == 1)) {
            Py_INCREF(kw);
            self->kw = kw;
        } else {
            self->kw = PyDict_Copy(kw);
        }
    } else {
        self->kw = PyDict_Copy(pkw);
        if (kw != NULL && self->kw != NULL) {
            if (PyDict_Merge(self->kw, kw, 1) != 0) {
                goto Fail;
            }
        }
    }

    if (self->kw == NULL) {
        goto Fail;
    }

    return (PyObject *)self;

Fail:
    if (startslice != 1) {
        Py_DECREF(pargs);
    }
    Py_XDECREF(self);
    return NULL;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
partial_call(PyIUObject_Partial *self,
             PyObject *args,
             PyObject *kw)
{
    PyObject *ret = NULL;
    PyObject *finalargs = NULL;
    PyObject *finalkw = NULL;
    Py_ssize_t num_placeholders = self->numph;
    Py_ssize_t selfargsize = PyTuple_GET_SIZE(self->args);
    Py_ssize_t passargsize = PyTuple_GET_SIZE(args);

    if (selfargsize == 0) {
        /* No own args, so we can simply use these passed to the function. */
        finalargs = args;
        Py_INCREF(args);
    } else if (passargsize == 0) {
        /* No passed arguments, we can simply reuse the own arguments except
           when these contain placeholders.
           */
        if (num_placeholders) {
            PyErr_SetString(PyExc_TypeError,
                            "not enough values to fill the placeholders in "
                            "`partial`.");
            goto Fail;
        }
        finalargs = self->args;
        Py_INCREF(self->args);
    } else {
        /* In case both the own arguments and the passed arguments contain
           at least one item we need to create a new tuple that contains them
           all (filling potential placeholders).
           */
        if (num_placeholders > passargsize) {
            PyErr_SetString(PyExc_TypeError,
                            "not enough values to fill the placeholders in "
                            "`partial`.");
            goto Fail;
        }
        /* In theory it would be possible to not create a new tuple for the
           call but only if the function doesn't keep that tuple (some
           functions could!). So probably best to always create a new tuple.
           */
        finalargs = PyTuple_New(selfargsize + passargsize - num_placeholders);
        if (finalargs == NULL) {
            return NULL;
        } else {
            Py_ssize_t i, j;
            /* Copy the elements from the self->args into the new tuple
               including the placeholders.
               */
            for (i=0 ; i<selfargsize ; i++) {
                PyObject *tmp = PyTuple_GET_ITEM(self->args, i);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(finalargs, i, tmp);
            }
            /* Replace the placeholders with the first items of the passed
               arguments. This doesn't decrement the reference count for the
               placeholders yet.
               */
            for (i=0 ; i<num_placeholders ; i++) {
                PyObject *tmp = PyTuple_GET_ITEM(args, i);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(finalargs, self->posph[i], tmp);
            }
            /* Now decrement the placeholders. */
            for (i=0 ; i<num_placeholders ; i++) {
                Py_DECREF(PYIU_Placeholder);
            }
            /* Now insert the remaining items of the passed arguments into the
               final tuple.
               */
            for (i=num_placeholders, j=selfargsize ; i<passargsize ; i++, j++) {
                PyObject *tmp = PyTuple_GET_ITEM(args, i);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(finalargs, j, tmp);
            }
        }
    }

    if (PyDict_Size(self->kw) == 0) {
        finalkw = kw;
        Py_XINCREF(finalkw);
    } else {
        finalkw = PyDict_Copy(self->kw);
        if (finalkw == NULL) {
            goto Fail;
        }
        if (kw != NULL) {
            if (PyDict_Merge(finalkw, kw, 1) != 0) {
                goto Fail;
            }
        }
    }

    /* Actually call the function. */
    ret = PyObject_Call(self->fn, finalargs, finalkw);

Fail:
    Py_XDECREF(finalargs);
    Py_XDECREF(finalkw);
    return ret;
}

#if PYIU_PYPY || PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3)

/******************************************************************************
 * __dict__ getter and setter
 *
 * only needed for python2 or python3 < 3.3 because later there are generic
 * options available
 *****************************************************************************/

static PyObject *
partial_get_dict(PyIUObject_Partial *self, void *Py_UNUSED(closure))
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
partial_set_dict(PyIUObject_Partial *self,
                 PyObject *value,
                 void *Py_UNUSED(closure))
{
    PyObject *tmp;

    /* It is illegal to del p.__dict__ */
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "a `partial` object's dictionary may not be deleted");
        return -1;
    }
    /* Can only set __dict__ to a dictionary */
    if (!PyDict_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "setting `partial` object's dictionary to a non-dict");
        return -1;
    }
    tmp = self->dict;
    Py_INCREF(value);
    self->dict = value;
    Py_XDECREF(tmp);
    return 0;
}
#endif

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
partial_repr(PyIUObject_Partial *self)
{
    PyObject *result = NULL;
    PyObject *arglist;
    Py_ssize_t i, n;
    PyObject *key, *value;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
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

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
partial_reduce(PyIUObject_Partial *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(O)(OOOO)", Py_TYPE(self), self->fn, self->fn,
                         self->args, self->kw,
                         self->dict ? self->dict : Py_None);
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
partial_setstate(PyIUObject_Partial *self,
                 PyObject *state)
{
    PyObject *fn, *fnargs, *kw, *dict;

    if (!PyTuple_Check(state) ||
            !PyArg_ParseTuple(state, "OOOO", &fn, &fnargs, &kw, &dict) ||
            !PyCallable_Check(fn) ||
            !PyTuple_Check(fnargs) ||
            (kw != Py_None && !PyDict_Check(kw))) {
        PyErr_SetString(PyExc_TypeError, "invalid `partial` state");
        return NULL;
    }

    if(!PyTuple_CheckExact(fnargs)) {
        fnargs = PySequence_Tuple(fnargs);
    } else {
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
    Py_XINCREF(dict);

    Py_CLEAR(self->fn);
    Py_CLEAR(self->args);
    Py_CLEAR(self->kw);
    Py_CLEAR(self->dict);
    self->fn = fn;
    self->args = fnargs;
    self->kw = kw;
    self->dict = dict;

    /* Free potentially existing array of positions and recreate it. */
    if (self->posph != NULL) {
        PyMem_Free(self->posph);
    }
    self->numph = PyIUPlaceholder_NumInTuple(self->args);
    if (self->numph) {
        self->posph = PyIUPlaceholder_PosInTuple(self->args, self->numph);
        if (self->posph == NULL) {
            return NULL;
        }
    } else {
        self->posph = NULL;
    }
    Py_RETURN_NONE;
}

/******************************************************************************
 * Sizeof
 *****************************************************************************/

static PyObject *
partial_sizeof(PyIUObject_Partial *self,
               PyObject *Py_UNUSED(args))
{
    Py_ssize_t res;
    res = sizeof(PyIUObject_Partial);
    /* Include the size of the posph array. */
    res += self->numph * sizeof(Py_ssize_t);
#if PY_MAJOR_VERSION == 2
    return PyInt_FromSsize_t(res);
#else
    return PyLong_FromSsize_t(res);
#endif
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef partial_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)partial_reduce,                       /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)partial_setstate,                     /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {"__sizeof__",                                      /* ml_name */
     (PyCFunction)partial_sizeof,                       /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_sizeof_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#if PYIU_PYPY || PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3)

static PyGetSetDef partial_getsetlist[] = {

    {"__dict__",                                        /* name */
     (getter)partial_get_dict,                          /* get */
     (setter)partial_set_dict,                          /* set */
     partial_prop___dict___doc,                         /* doc */
     (void *)NULL                                       /* closure */
     },

    {NULL}                                              /* sentinel */
};

#else

static PyGetSetDef partial_getsetlist[] = {

    {"__dict__",                                        /* name */
     PyObject_GenericGetDict,                           /* get */
     PyObject_GenericSetDict,                           /* set */
     partial_prop___dict___doc,                         /* doc */
     (void *)NULL                                       /* closure */
     },

    {NULL}                                              /* sentinel */
};

#endif

#define OFF(x) offsetof(PyIUObject_Partial, x)
static PyMemberDef partial_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(fn),                                           /* offset */
     READONLY,                                          /* flags */
     partial_prop_func_doc                              /* doc */
     },

    {"args",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(args),                                         /* offset */
     READONLY,                                          /* flags */
     partial_prop_args_doc                              /* doc */
     },

    {"keywords",                                        /* name */
     T_OBJECT,                                          /* type */
     OFF(kw),                                           /* offset */
     READONLY,                                          /* flags */
     partial_prop_keywords_doc                          /* doc */
     },

    {"num_placeholders",                                /* name */
     T_PYSSIZET,                                        /* type */
     OFF(numph),                                        /* offset */
     READONLY,                                          /* flags */
     partial_prop_nplaceholders_doc                     /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Partial = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.partial",        /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Partial),             /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)partial_dealloc,                        /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)partial_repr,                             /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)partial_call,                          /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)PyObject_GenericSetAttr,              /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)partial_doc,                          /* tp_doc */
    (traverseproc)partial_traverse,                     /* tp_traverse */
    (inquiry)partial_clear,                             /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)offsetof(PyIUObject_Partial, weakreflist),/* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    partial_methods,                                    /* tp_methods */
    partial_memberlist,                                 /* tp_members */
    partial_getsetlist,                                 /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)offsetof(PyIUObject_Partial, dict),     /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)partial_new,                               /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};

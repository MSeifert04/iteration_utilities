/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 *
 * IMPORTANT NOTE:
 *
 * The function does the same as "heapq.merge(*iterables)" or
 * "sorted(itertools.chain(*iterables))" it is included because heapq.merge
 * did not have the key and reverse parameter before Python 3.5 and it is
 * included for compatibility reasons.
 *
 * That this is (much) faster than heapq.merge for most inputs is a nice but
 * worrying side effect. :-(
 *
 *****************************************************************************/



/******************************************************************************
 * ------------------------------- HELPER -------------------------------------
 *
 * Find the position to insert a value in an already sorted tuple. Assumes that
 * the sorting should be stable and searches the rightmost place where the
 * tuple is still sorted.
 *
 * Function will compare first to the "hi-1"-th element and then start
 * bisecting. (See inline code for explanation).
 *
 * tuple : Sorted tuple to inspect
 * item  : Value to search the position for.
 * hi    : Upper index to search for.
 * cmpop : The comparison operator to use. For example Py_LT for a tuple sorted
 *         from low to high or Py_GT for a tuple sorted from high to low.
 *
 * Returns -1 on failure otherwise a positive Py_ssize_t value.
 *
 * Copied and modified from the python bisect module.
 *****************************************************************************/

Py_ssize_t
PyIU_TupleBisectRight_LastFirst(PyObject *tuple,
                                PyObject *item,
                                Py_ssize_t hi,
                                int cmpop)
{
    /* Temporary variables */
    PyObject *litem;
    int res;

    /* Indices for the left end and mid of the current part of the array.
       The right end (hi) is given as input.
       */
    Py_ssize_t mid, lo = 0;

    /* Bisection has two worst cases: If it should be inserted in the first or
       last place. The list is reverse-ordered so it's likely that the
       bisection could return the last place (for bisect_left it would be the
       first) in the "merge_sorted" function.

       Checking the number of comparisons in "merge" shows that merge now uses
       slightly less comparisons than "sorted" in the average case, slightly
       more in the worst case and much less in the best case! */

    /* So let's check the last item first! */
    if (hi <= 0) {
        return 0;
    }
    litem = PyTuple_GET_ITEM(tuple, hi-1);
    res = PyIU_ItemIdxKey_Compare(item, litem, cmpop);
    if (res == 1) {
        return hi;
    } else if (res == 0) {
        hi = hi - 1;
    } else {
        return -1;
    }

    /* Start the normal bisection algorithm from biscet.c */
    while (lo < hi) {
        mid = ((size_t)lo + hi) / 2;
        litem = PyTuple_GET_ITEM(tuple, mid);
        res = PyIU_ItemIdxKey_Compare(item, litem, cmpop);
        if (res == 1) {
            lo = mid + 1;
        } else if (res == 0) {
            hi = mid;
        } else {
            return -1;
        }
    }
    return lo;
}



typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    PyObject *keyfunc;
    PyObject *current;
    Py_ssize_t numactive;
    int reverse;
    PyObject *funcargs;
} PyIUObject_Merge;

PyTypeObject PyIUType_Merge;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
merge_new(PyTypeObject *type,
          PyObject *args,
          PyObject *kwargs)
{
    static char *kwlist[] = {"key", "reverse", NULL};
    PyIUObject_Merge *self;

    PyObject *iteratortuple = NULL;
    PyObject *keyfunc = NULL;
    PyObject *funcargs = NULL;
    int reverse = 0;

    /* Parse arguments */

    if (!PyArg_ParseTupleAndKeywords(PyIU_global_0tuple, kwargs,
                                     "|OO:merge", kwlist,
                                     &keyfunc, &reverse)) {
        return NULL;
    }

    reverse = reverse ? Py_GT : Py_LT;

    PYIU_NULL_IF_NONE(keyfunc);
    Py_XINCREF(keyfunc);

    /* Create and fill struct */
    iteratortuple = PyIU_CreateIteratorTuple(args);
    if (iteratortuple == NULL) {
        goto Fail;
    }

    if (keyfunc != NULL) {
        funcargs = PyTuple_New(1);
        if (funcargs == NULL) {
            goto Fail;
        }
    }

    self = (PyIUObject_Merge *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }

    self->iteratortuple = iteratortuple;
    self->keyfunc = keyfunc;
    self->reverse = reverse;
    self->current = NULL;
    self->numactive = PyTuple_GET_SIZE(args);
    self->funcargs = funcargs;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iteratortuple);
    Py_XDECREF(keyfunc);
    Py_XDECREF(funcargs);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
merge_dealloc(PyIUObject_Merge *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratortuple);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->current);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
merge_traverse(PyIUObject_Merge *self,
               visitproc visit,
               void *arg)
{
    Py_VISIT(self->iteratortuple);
    Py_VISIT(self->keyfunc);
    Py_VISIT(self->current);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Initialize "current"
 *****************************************************************************/

static int
merge_init_current(PyIUObject_Merge *self)
{
    PyObject *current, *iterator, *item, *newitem=NULL, *keyval=NULL;
    Py_ssize_t i, insert, tuplelength;

    current = PyTuple_New(self->numactive);
    if (current == NULL) {
        return -1;
    }
    tuplelength = 0;

    for (i=0 ; i<self->numactive ; i++) {
        iterator = PyTuple_GET_ITEM(self->iteratortuple, i);
        item = (*Py_TYPE(iterator)->tp_iternext)(iterator);
        if (item != NULL) {
            /* The idea here is that we can keep stability by also remembering
               the index of the iterable (which is also useful to remember
               from which iterable to get the next item if it is yielded).
               */
            if (self->keyfunc != NULL) {
                PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, Py_DECREF(current);
                                                             Py_DECREF(item);
                                                             return -1;);
                keyval = PyObject_Call(self->keyfunc, self->funcargs, NULL);
                if (keyval == NULL) {
                    Py_DECREF(item);
                    goto Fail;
                }
            }
            newitem = PyIU_ItemIdxKey_FromC(item, i, keyval);

            /* Insert the tuple into the current tuple. */
            if (tuplelength == 0) {
                PyTuple_SET_ITEM(current, 0, newitem);
            } else {
                insert = PyIU_TupleBisectRight_LastFirst(current, newitem,
                                                         tuplelength,
                                                         self->reverse);
                if (insert < 0) {
                    goto Fail;
                }
                PyIU_TupleInsert(current, insert, newitem, tuplelength+1);
            }
            tuplelength++;
        } else {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }
        }
    }
    self->numactive = tuplelength;
    self->current = current;
    return 0;

Fail:
    Py_DECREF(current);
    Py_XDECREF(newitem);
    return -1;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
merge_next(PyIUObject_Merge *self)
{
    PyObject *iterator, *item, *val, *keyval, *oldkeyval;
    Py_ssize_t insert = 0;
    Py_ssize_t active;
    PyIUObject_ItemIdxKey *next;

    /* No current then we create one. */
    if (self->current == NULL) {
        if (merge_init_current(self) < 0) {
            return NULL;
        }
    }

    /* Finished as soon as there are no more active iterators. */
    if (self->numactive == 0) {
        return NULL;
    }

    active = self->numactive - 1;

    /* Tuple containing the next value. */
    next = (PyIUObject_ItemIdxKey *)PyTuple_GET_ITEM(self->current, active);
    Py_INCREF(next);

    /* Value to be returned. */
    val = next->item;
    Py_INCREF(val);

    /* Get the next value from the iterable where the value was from.  */
    iterator = PyTuple_GET_ITEM(self->iteratortuple, next->idx);
    item = (*Py_TYPE(iterator)->tp_iternext)(iterator);

    if (item == NULL) {
        /* No need to keep the extra reference for the ItemIdxKey because there
           is no successive value and we replace the item in the current tuple
           with NULL.
           */
        PyTuple_SET_ITEM(self->current, active, NULL);
        Py_DECREF(next);
        Py_DECREF(next);
        if (PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
            } else {
                Py_DECREF(val);
                return NULL;
            }
        }
        Py_INCREF(val);
        self->numactive = active;
    } else {
        if (self->keyfunc != NULL) {
            oldkeyval = next->key;
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, Py_DECREF(item);
                                                         Py_DECREF(val);
                                                         Py_DECREF(next);
                                                         return NULL;);
            keyval = PyObject_Call(self->keyfunc, self->funcargs, NULL);
            if (keyval == NULL) {
                Py_DECREF(item);
                Py_DECREF(val);
                Py_DECREF(next);
                return NULL;
            }
            next->key = keyval;
            next->item = item;
            Py_DECREF(oldkeyval);
        } else {
            next->item = item;
        }

        /* Insert the new value into the sorted current tuple. */
        insert = PyIU_TupleBisectRight_LastFirst(self->current, (PyObject *)next,
                                                 active, self->reverse);
        if (insert == -1) {
            Py_DECREF(next);
            Py_DECREF(next);
            Py_DECREF(val);
            Py_DECREF(val);
            return NULL;
        }
        PyIU_TupleInsert(self->current, insert, (PyObject *)next, self->numactive);
        Py_DECREF(next);
    }
    Py_DECREF(val);
    return val;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
merge_reduce(PyIUObject_Merge *self)
{
    PyObject *res;
    PyObject *current;

    /* We need to expose the "current" tuple. However this tuple is modifed
       when calling next so we need a copy, otherwise people would have a
       mutable tuple. That must NOT happen!
       In case the number of elements in the tuple differs from the "numactive"
       attribute we can simply slice the trailing NULLs away.
       The "ItemIdxKey" instances inside the "current" tuple are mutable so
       we need to make sure these cannot be altered from outside. So we need
       to make more than a shallow copy...

       The "iteratortuple" isn't changed in the "next" call so we can simply
       expose it as-is.
       */
    if (self->current == NULL) {
        current = Py_None;
        Py_INCREF(current);
    } else {
        Py_ssize_t i;
        current = PyTuple_New(self->numactive);
        if (current == NULL) {
            return NULL;
        }
        for (i=0 ; i < self->numactive ; i++) {
            PyObject *iik1 = PyTuple_GET_ITEM(self->current, i);
            PyObject *iik2 = PyIU_ItemIdxKey_Copy(iik1);
            if (iik2 == NULL) {
                return NULL;
            }
            PyTuple_SET_ITEM(current, i, iik2);
        }
    }
    /* No need to copy the iteratortuple because we don't modify it anywhere
       so we can easily get away by having more than one reference for it.
       */
    res = Py_BuildValue("OO(OiOn)", Py_TYPE(self),
                        self->iteratortuple,
                        self->keyfunc ? self->keyfunc : Py_None,
                        self->reverse,
                        current,
                        self->numactive);
    Py_DECREF(current);
    return res;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
merge_setstate(PyIUObject_Merge *self,
               PyObject *state)
{
    PyObject *current, *keyfunc;
    Py_ssize_t numactive;
    int reverse;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "OiOn:merge.__setstate__",
                          &keyfunc, &reverse, &current, &numactive)) {
        return NULL;
    }

    PYIU_NULL_IF_NONE(current);
    PYIU_NULL_IF_NONE(keyfunc);

    /* If it's from a "reduce" call then it should work fine, but if someone
       tries to feed anything in here we need to check the conditions the
       next is based on:

       - 0 <= numactive <= len(iteratortuple) == len(current) (except when no
         current is initialized)
       - current may only contain ItemIdxKey instances
         - These must have NO key-attribute when keyfunc==NULL
         - These must have A key-attribute when keyfunc!=NULL
         - These must not have an idx that is out of range for the iteratortuple

       These tests only make sure the function does not crash, the inputs may
       result in useless results!
       */

    /* "numactive" must be positive and <= len(self->iteratortuple) otherwise
        item access might segfault.
       */
    if (numactive < 0 || numactive > PyTuple_GET_SIZE(self->iteratortuple)) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the fourth (%zd) "
                     "argument in the `state` is not negative and is smaller "
                     "or equal to the number of iterators (%zd).",
                     Py_TYPE(self)->tp_name,
                     numactive,
                     PyTuple_GET_SIZE(self->iteratortuple));
        return NULL;
    }

    if (current != NULL) {
        Py_ssize_t i;
        Py_ssize_t currentsize;

        /* current must be a tuple, otherwise the PyTuple_GET_ITEM and
           PyTuple_SET_ITEM operations in "next" will segfault.
           */
        if (!PyTuple_CheckExact(current)) {
            PyErr_Format(PyExc_TypeError,
                         "`%.200s.__setstate__` expected a `tuple` instance as "
                         "third argument in the `state`, got %.200s.",
                         Py_TYPE(self)->tp_name, Py_TYPE(current)->tp_name);
            return NULL;
        }
        /* The length of the current tuple and the "numactive" value must be
           identical, otherwise this might loose items (numactive smaller) or
           segfault (numactive bigger) because it is used to index this tuple.
           */
        currentsize = PyTuple_GET_SIZE(current);
        if (currentsize != numactive) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the fourth (%zd) "
                         "argument in the `state` is equal to the length of "
                         "the third argument (%zd).",
                         Py_TYPE(self)->tp_name, numactive, currentsize);
            return NULL;
        }

        /* Unfortunatly we have to check each item in the "current" tuple
           to make sure the "next" function doesn't segfault.
           - Each item must be an "ItemIdxKey" item.
           - Each ItemIdxKey must have a key attribute if we have a keyfunction
             or mustn't have a key if we have no key function.
           - Each ItemIdxKey idx attribute must have a value that isn't out
             of bounds for the iteratortuple.
           There are some additional checks that could be done but aren't
           because they might have side-effects and could slow down this
           function unnecessarily:
           - The "idx" attribute of the ItemIdxKey instances should be unique
             within the "current" tuple. Meaning there shouldn't be more than
             one pointing to the same iterator.
           - The "current" tuple is supposed to be sorted (either decreasing
             if "reverse=False" or increasing otherwise) and using an unsorted
             "current" will break the function. However this requirement isn't
             enforced for the iterators when they are passed in so there is
             actually already a way to "break" the function.
           */
        for (i=0 ; i < currentsize ; i++) {
            Py_ssize_t idx;
            PyObject *iik = PyTuple_GET_ITEM(current, i);

            if (!PyIU_ItemIdxKey_CheckExact(iik)) {
                PyErr_Format(PyExc_TypeError,
                             "`%.200s.__setstate__` expected that the third "
                             "argument in the `state` contains only "
                             "`ItemIdxKey` instances, got %.200s.",
                             Py_TYPE(self)->tp_name, Py_TYPE(iik)->tp_name);
                return NULL;
            }

            if (keyfunc == NULL) {
                if (((PyIUObject_ItemIdxKey *)iik)->key != NULL) {
                    PyErr_Format(PyExc_TypeError,
                                 "`%.200s.__setstate__` expected that `ItemIdxKey` "
                                 "instances in the third argument in the `state` "
                                 "have no `key` attribute when the first argument "
                                 "is `None`.",
                                 Py_TYPE(self)->tp_name);
                    return NULL;
                }
            } else {
                if (((PyIUObject_ItemIdxKey *)iik)->key == NULL) {
                    PyErr_Format(PyExc_TypeError,
                                 "`%.200s.__setstate__` expected that `ItemIdxKey` "
                                 "instances in the third argument in the `state` "
                                 "have a `key` attribute when the first argument "
                                 "is not `None`.",
                                 Py_TYPE(self)->tp_name);
                    return NULL;
                }
            }

            idx = ((PyIUObject_ItemIdxKey *)iik)->idx;
            if (idx < 0 || idx >= PyTuple_GET_SIZE(self->iteratortuple)) {
                PyErr_Format(PyExc_ValueError,
                             "`%.200s.__setstate__` expected that `ItemIdxKey` "
                             "instances in the third argument in the `state` "
                             "have a `idx` attribute (%zd) that is smaller than "
                             "the length of the `iteratortuple` (%zd)",
                             Py_TYPE(self)->tp_name,
                             idx,
                             PyTuple_GET_SIZE(self->iteratortuple));
                return NULL;
            }
        }
    }

    /* We need to make sure to copy the "current" because we will alter this
       tuple inside the "next" calls. If someone would hold a reference their
       tuple would change. This should never happen!
       Also the ItemIdxKey instances are mutable from outside so these have
       to be copied as well.
       */
    if (current != NULL) {
        Py_ssize_t i;
        PyObject *new_current = PyTuple_New(numactive);
        if (new_current == NULL) {
            return NULL;
        }
        for (i=0 ; i < numactive ; i++) {
            PyObject *iik1 = PyTuple_GET_ITEM(current, i);
            PyObject *iik2 = PyIU_ItemIdxKey_Copy(iik1);
            if (iik2 == NULL) {
                return NULL;
            }
            PyTuple_SET_ITEM(new_current, i, iik2);
        }
        current = new_current;
    }

    /* If we have a keyfunc we need to create funcargs if the class doesn't
       have them already.
       */
    if (keyfunc != NULL && self->funcargs == NULL) {
        PyObject *funcargs = PyTuple_New(1);
        if (funcargs == NULL) {
            return NULL;
        }
        Py_CLEAR(self->funcargs);
        self->funcargs = funcargs;
    }

    Py_CLEAR(self->keyfunc);
    self->keyfunc = keyfunc;
    Py_XINCREF(self->keyfunc);

    Py_CLEAR(self->current);
    self->current = current;
    /* No need to incref the "current" because we copied it already! */

    self->numactive = numactive;
    self->reverse = reverse;
    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
merge_lengthhint(PyIUObject_Merge *self)
{
    Py_ssize_t i, len = 0;
    if (self->current == NULL) {
        for (i=0 ; i<PyTuple_GET_SIZE(self->iteratortuple) ; i++) {
            len += PyObject_LengthHint(PyTuple_GET_ITEM(self->iteratortuple, i), 0);
        }
    } else {
        len += self->numactive;
        for (i=0 ; i<self->numactive ; i++) {
            PyObject *iik = PyTuple_GET_ITEM(self->current, i);
            Py_ssize_t idx = ((PyIUObject_ItemIdxKey *)iik)->idx;
            PyObject *it = PyTuple_GET_ITEM(self->iteratortuple, idx);
            len += PyObject_LengthHint(it, 0);
        }
    }

    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef merge_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)merge_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__",   (PyCFunction)merge_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)merge_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(merge_doc, "merge(*iterables, /, key=None, reverse=False)\n\
--\n\
\n\
Merge sorted `iterables` into one.\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    Any amount of already sorted `iterable` objects.\n\
\n\
key : callable or None, optional\n\
    If not given compare the item themselves otherwise compare the\n\
    result of ``key(item)``, like the `key` parameter for\n\
    :py:func:`sorted`.\n\
\n\
reverse : boolean, optional\n\
    If ``True`` then merge in decreasing order instead of increasing order.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
merged : generator\n\
    The merged iterables as generator.\n\
\n\
See also\n\
--------\n\
heapq.merge : Equivalent since Python 3.5 but in most cases slower!\n\
    Earlier Python versions did not support the `key` or `reverse` argument.\n\
\n\
sorted : ``sorted(itertools.chain(*iterables))`` supports the same options\n\
    and *can* be faster.\n\
\n\
Examples\n\
--------\n\
To merge multiple sorted `iterables`::\n\
\n\
    >>> from iteration_utilities import merge\n\
    >>> list(merge([1, 3, 5, 7, 9], [2, 4, 6, 8, 10]))\n\
    [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]\n\
\n\
It's stable and allows a `key` function::\n\
\n\
    >>> seq1 = [(1, 3), (3, 3)]\n\
    >>> seq2 = [(-1, 3), (-3, 3)]\n\
    >>> list(merge(seq1, seq2, key=lambda x: abs(x[0])))\n\
    [(1, 3), (-1, 3), (3, 3), (-3, 3)]\n\
\n\
Also possible to `reverse` (biggest to smallest order) the merge::\n\
\n\
    >>> list(merge([5,1,-8], [10, 2, 1, 0], reverse=True))\n\
    [10, 5, 2, 1, 1, 0, -8]\n\
\n\
But also more than two `iterables`::\n\
\n\
    >>> list(merge([1, 10, 11], [2, 9], [3, 8], [4, 7], [5, 6], range(10)))\n\
    [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11]\n\
\n\
However if the `iterabes` are not sorted the result will be unsorted\n\
(partially sorted)::\n\
\n\
    >>> list(merge(range(10), [6,1,3,2,6,1,6]))\n\
    [0, 1, 2, 3, 4, 5, 6, 6, 1, 3, 2, 6, 1, 6, 7, 8, 9]");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Merge = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.merge",                        /* tp_name */
    sizeof(PyIUObject_Merge),                           /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)merge_dealloc,                          /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    merge_doc,                                          /* tp_doc */
    (traverseproc)merge_traverse,                       /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)merge_next,                           /* tp_iternext */
    merge_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    merge_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};

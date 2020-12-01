/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "partial.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "docs_sizeof.h"
#include "helper.h"
#include "placeholder.h"

PyDoc_STRVAR(
    partial_prop_func_doc,
    "(callable) Function object to use in future partial calls (readonly).");

PyDoc_STRVAR(
    partial_prop_args_doc,
    "(:py:class:`tuple`) arguments for future partial calls (readonly).");

PyDoc_STRVAR(
    partial_prop_keywords_doc,
    "(:py:class:`dict`) keyword arguments for future partial calls (readonly).");

PyDoc_STRVAR(
    partial_prop_nplaceholders_doc,
    "(:py:class:`int`) Number of placeholders in the args (readonly).");

PyDoc_STRVAR(partial_prop___dict___doc, "");

PyDoc_STRVAR(
    partial_doc,
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
    ":py:const:`~iteration_utilities.Placeholder` can be used as placeholders \n"
    "for the positional arguments.\n"
    "\n"
    "For example most iterators in :py:mod:`iteration_utilities` take the `iterable` \n"
    "as the first argument so other arguments can be easily added::\n"
    "\n"
    "    >>> from iteration_utilities import accumulate, Placeholder\n"
    "    >>> from operator import mul\n"
    "    >>> cumprod = partial(accumulate, Placeholder, mul)\n"
    "    >>> list(cumprod([1,2,3,4,5]))\n"
    "    [1, 2, 6, 24, 120]\n");

#if PyIU_USE_VECTORCALL
static PyObject *partial_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames);
#endif

/******************************************************************************
 * Helper to get the amount and positions of Placeholders in a tuple.
 *****************************************************************************/

static Py_ssize_t
PyIUPlaceholder_NumInTuple(PyObject *tup) {
    Py_ssize_t cnts = 0;
    Py_ssize_t i;

    /* Find the placeholders (if any) in the tuple. */
    for (i = 0; i < PyTuple_GET_SIZE(tup); i++) {
        if (PyTuple_GET_ITEM(tup, i) == PYIU_Placeholder) {
            cnts++;
        }
    }

    return cnts;
}

static Py_ssize_t *
PyIUPlaceholder_PosInTuple(PyObject *tup, Py_ssize_t cnts) {
    assert(cnts >= 0);
    Py_ssize_t j = 0;
    Py_ssize_t i;

    Py_ssize_t *pos = PyMem_Malloc((size_t)cnts * sizeof(Py_ssize_t));
    if (pos == NULL) {
        PyErr_SetNone(PyExc_MemoryError);
        return NULL;
    }

    /* Find the placeholders (if any) in the tuple. */
    for (i = 0; i < PyTuple_GET_SIZE(tup); i++) {
        if (PyTuple_GET_ITEM(tup, i) == PYIU_Placeholder) {
            pos[j] = i;
            j++;
        }
    }

    if (j != cnts) {
        PyErr_SetString(PyExc_TypeError,
                        "Something went wrong... totally wrong!");
        PyMem_Free(pos);
        return NULL;
    }

    return pos;
}

/******************************************************************************
 * Parts are taken from the CPython package (PSF licensed).
 *****************************************************************************/

static void
partial_dealloc(PyIUObject_Partial *self) {
    PyObject_GC_UnTrack(self);
    if (self->weakreflist != NULL) {
        PyObject_ClearWeakRefs((PyObject *)self);
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

static int
partial_traverse(PyIUObject_Partial *self, visitproc visit, void *arg) {
    Py_VISIT(self->fn);
    Py_VISIT(self->args);
    Py_VISIT(self->kw);
    Py_VISIT(self->dict);
    return 0;
}

static int
partial_clear(PyIUObject_Partial *self) {
    Py_CLEAR(self->fn);
    Py_CLEAR(self->args);
    Py_CLEAR(self->kw);
    Py_CLEAR(self->dict);
    /* TODO: Is it necessary to clear the self->posph array here? Probably not
             because it doesn't contain PyObjects ...
       */
    return 0;
}

static PyObject *
partial_new(PyTypeObject *type, PyObject *args, PyObject *kw) {
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
    if (PyIU_IsTypeExact(func, &PyIUType_Partial) &&
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
            for (i = 0; i < stop; i++) {
                PyObject *tmp = PyTuple_GET_ITEM(args, i + 1);
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
        startslice = 1; /* So the "Fail" won't decrement them again. */
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

#if PyIU_USE_VECTORCALL
    self->vectorcall = partial_vectorcall;
#endif

    return (PyObject *)self;

Fail:
    if (startslice != 1) {
        Py_DECREF(pargs);
    }
    Py_XDECREF(self);
    return NULL;
}

#if PyIU_USE_VECTORCALL

static PyObject *
partial_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames) {
    PyObject *small_stack[PyIU_SMALL_ARG_STACK_SIZE];
    PyObject **stack = small_stack;
    PyIUObject_Partial *self = (PyIUObject_Partial *)obj;
    Py_ssize_t n_args = PyVectorcall_NARGS(nargsf);
    Py_ssize_t n_kwargs = kwnames == NULL ? 0 : PyTuple_GET_SIZE(kwnames);
    Py_ssize_t n_self_args = PyTuple_GET_SIZE(self->args);
    Py_ssize_t n_self_kwargs = PyDict_Size(self->kw);
    Py_ssize_t n_duplicate_kwargs = 0;
    PyObject *kwnames_lookup = kwnames;
    PyObject *final_kwnames = NULL;
    PyObject *result = NULL;

    if (n_args < self->numph) {
        PyErr_SetString(PyExc_TypeError,
                        "not enough values to fill the placeholders in "
                        "`partial`.");
        return NULL;
    }
    Py_ssize_t n_final_args = n_self_args + n_args - self->numph;
    Py_ssize_t n_final_kwargs = n_self_kwargs + n_kwargs;
    Py_ssize_t n_final = n_final_args + n_final_kwargs;

    /* Since n_final doesn't account for duplicate keyword arguments in
       self->kw and kwnames this will be an overestimate. But I think an
       overestimate is good enough in most cases. */
    if (n_final > PyIU_SMALL_ARG_STACK_SIZE) {
        stack = PyIU_AllocatePyObjectArray(n_final);
        if (stack == NULL) {
            return PyErr_NoMemory();
        }
    }

    // Fill args
    PyIU_CopyTupleToArray(self->args, stack, (size_t)n_self_args);
    // Fill placeholders
    Py_ssize_t idx;
    for (idx = 0; idx < self->numph; idx++) {
        stack[self->posph[idx]] = args[idx];
    }
    // Fill remaining additional args
    memcpy(stack + n_self_args, args + self->numph, (n_args - self->numph) * sizeof(PyObject *));

    Py_ssize_t current_idx = n_final_args;

    /* In the following we need to ensure that we don't let arbitrary Python code
       run which might alter the instance.
       Since we're potentially using the __hash__ and __eq__ of the keyword
       names the keywords must be unicodes (not subclasses)!
       The self->kw should always be a real dictionary, so there's (probably) no
       way this could trigger Python code while iterating over it.
       */
    if (kwnames != NULL) {
        Py_ssize_t kwname_idx;
        for (kwname_idx = 0; kwname_idx < n_kwargs; kwname_idx++) {
            PyObject *kwname = PyTuple_GET_ITEM(kwnames, kwname_idx);
            if (!PyUnicode_CheckExact(kwname)) {
                PyErr_SetString(PyExc_TypeError, "keyword names must be strings.");
                goto CleanUp;
            }
        }
    }
    /* The check that all keyword names are strings is done in one of the following loops. */

    /* In case we have both kwargs in the partial and in the partial call we
       have to check for duplicates. To ensure that the call doesn't suffer from
       the quadratic lookup behavior when checking if each kwarg in the instance
       is also present in the kwnames-TUPLE this creates a set for the kwnames
       in case we have more than X values in the instance kwargs and more than
       Y values in the passed kwargs. The values chosen here aren't based on
       any empirical testing they are just educated guesses. This is probably
       unnecessary because there will likely be very rare that so many kwargs
       are in the instance and in the actual call.
       */
    if (n_self_kwargs > 5 && n_kwargs >= 10) {
        kwnames_lookup = PyFrozenSet_New(kwnames);
        if (kwnames_lookup == NULL) {
            goto CleanUp;
        }
    }
    // Fill in the keywords stored in the instance.
    if (n_self_kwargs != 0) {
        PyObject *key;
        PyObject *value;
        Py_ssize_t pos = 0;

        if (kwnames == NULL) {
            /* No keyword arguments when the partial is called, we can simply
               use the values.*/
            while (PyDict_Next(self->kw, &pos, &key, &value)) {
                if (PyUnicode_CheckExact(key)) {
                    stack[current_idx] = value;
                    current_idx++;
                } else {
                    PyErr_SetString(PyExc_TypeError, "keyword names must be strings.");
                    goto CleanUp;
                }
            }
        } else {
            while (PyDict_Next(self->kw, &pos, &key, &value)) {
                if (PyUnicode_CheckExact(key)) {
                    int ok;
                    ok = PySequence_Contains(kwnames_lookup, key);
                    if (ok == 1) {
                        /* The keyword is also present in the call. Skip it. */
                        n_duplicate_kwargs++;
                    } else if (ok == 0) {
                        /* The keyword is not present in the call. */
                        stack[current_idx] = value;
                        current_idx++;
                    } else {
                        /* It's very unlikely that there will be a lookup failure
                        since the kwargs for the instance and the kwargs for the
                        call are already validated by Pythons function call
                        infrastructure. However better to have this in-place in
                        case it really ever happens...
                        */
                        goto CleanUp;
                    }
                } else {
                    PyErr_SetString(PyExc_TypeError, "keyword names must be strings.");
                    goto CleanUp;
                }
            }
        }
    }
    /* No special treatment for the keyword arguments passed to the call of the
       partial. We can simply add them to the stack. */
    if (n_kwargs != 0) {
        Py_ssize_t arg_idx = n_args;
        for (arg_idx = n_args; arg_idx < n_args + n_kwargs; arg_idx++) {
            stack[current_idx] = args[arg_idx];
            current_idx++;
        }
    }

    /* 4 cases:
       - self->kwargs && kwargs
       - self->kwargs && not kwargs
       - not self->kwargs && kwargs
       - not self->kwargs && not kwargs

       The last two cases are easily treated because we can simply use the
       keyword names that are passed in this call.
       The first two require more special treatment because the instance kwargs
       are a dict and need to be converted to a tuple of kwnames.
       In case the call also has kwargs we also need to account for the duplicate
       keywords.
       */
    if (n_self_kwargs == 0) {
        final_kwnames = kwnames;
    } else {
        PyObject *key;
        PyObject *value;
        Py_ssize_t self_kwargs_pos = 0;
        Py_ssize_t final_kwnames_idx = 0;
        /* At this point we know the exact number of keyword arguments without
           duplicates. So we can create the tuple holding the keyword names.
           */
        final_kwnames = PyTuple_New(n_final_kwargs - n_duplicate_kwargs);
        if (final_kwnames == NULL) {
            goto CleanUp;
        }
        /* Fill in the keywords stored in the instance. This relies on the fact
           that the self->kw dictionary hasn't changed between the previous step
           where we added the values and this step where we use the keys.
           */
        if (n_duplicate_kwargs == 0) {
            while (PyDict_Next(self->kw, &self_kwargs_pos, &key, &value)) {
                Py_INCREF(key);
                PyTuple_SET_ITEM(final_kwnames, final_kwnames_idx, key);
                final_kwnames_idx++;
            }
        } else {
            while (PyDict_Next(self->kw, &self_kwargs_pos, &key, &value)) {
                int ok;
                ok = PySequence_Contains(kwnames_lookup, key);
                if (ok == 0) {
                    Py_INCREF(key);
                    PyTuple_SET_ITEM(final_kwnames, final_kwnames_idx, key);
                    final_kwnames_idx++;
                } else if (ok == 1) {
                    // Keyword is skipped.
                } else {
                    goto CleanUp;
                }
            }
        }
        /* Fill in the keyword argument names from the call. */
        if (kwnames != NULL) {
            Py_ssize_t kwnames_idx;
            for (kwnames_idx = 0; kwnames_idx < n_kwargs; kwnames_idx++) {
                PyObject *kwname = PyTuple_GET_ITEM(kwnames, kwnames_idx);
                Py_INCREF(kwname);
                PyTuple_SET_ITEM(final_kwnames, final_kwnames_idx, kwname);
                final_kwnames_idx++;
            }
        }
    }
    result = PyIU_PyObject_Vectorcall(self->fn, stack, n_final_args, final_kwnames);

CleanUp:
    if (stack != small_stack) {
        PyMem_Free(stack);
    }
    if (kwnames_lookup != kwnames) {
        Py_DECREF(kwnames_lookup);
    }
    if (final_kwnames != kwnames) {
        Py_XDECREF(final_kwnames);
    }
    return result;
}

#else

static PyObject *
partial_call(PyIUObject_Partial *self, PyObject *args, PyObject *kw) {
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
            for (i = 0; i < selfargsize; i++) {
                PyObject *tmp = PyTuple_GET_ITEM(self->args, i);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(finalargs, i, tmp);
            }
            /* Replace the placeholders with the first items of the passed
               arguments. This doesn't decrement the reference count for the
               placeholders yet.
               */
            for (i = 0; i < num_placeholders; i++) {
                PyObject *tmp = PyTuple_GET_ITEM(args, i);
                Py_INCREF(tmp);
                PyTuple_SET_ITEM(finalargs, self->posph[i], tmp);
            }
            /* Now decrement the placeholders. */
            for (i = 0; i < num_placeholders; i++) {
                Py_DECREF(PYIU_Placeholder);
            }
            /* Now insert the remaining items of the passed arguments into the
               final tuple.
               */
            for (i = num_placeholders, j = selfargsize; i < passargsize; i++, j++) {
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
#endif

#if PYIU_PYPY

/******************************************************************************
 * __dict__ getter and setter
 *
 * only needed for python2 or python3 < 3.3 because later there are generic
 * options available
 *****************************************************************************/

static PyObject *
partial_get_dict(PyIUObject_Partial *self, void *Py_UNUSED(closure)) {
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
partial_set_dict(PyIUObject_Partial *self, PyObject *value, void *Py_UNUSED(closure)) {
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

static PyObject *
partial_repr(PyIUObject_Partial *self) {
    PyObject *result = NULL;
    PyObject *arglist;
    PyObject *key;
    PyObject *value;
    Py_ssize_t n;
    Py_ssize_t i;
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
        Py_XSETREF(arglist, tmp);
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

        Py_XSETREF(arglist, tmp);
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

static PyObject *
partial_reduce(PyIUObject_Partial *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(O)(OOOO)", Py_TYPE(self), self->fn, self->fn,
                         self->args, self->kw,
                         self->dict ? self->dict : Py_None);
}

static PyObject *
partial_setstate(PyIUObject_Partial *self, PyObject *state) {
    PyObject *fn;
    PyObject *fnargs;
    PyObject *kw;
    PyObject *dict;

    if (!PyTuple_Check(state) ||
        !PyArg_ParseTuple(state, "OOOO", &fn, &fnargs, &kw, &dict) ||
        !PyCallable_Check(fn) ||
        !PyTuple_Check(fnargs) ||
        (kw != Py_None && !PyDict_Check(kw))) {
        PyErr_SetString(PyExc_TypeError, "invalid `partial` state");
        return NULL;
    }

    if (!PyTuple_CheckExact(fnargs)) {
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

    Py_XSETREF(self->fn, fn);
    Py_XSETREF(self->args, fnargs);
    Py_XSETREF(self->kw, kw);
    Py_XSETREF(self->dict, dict);

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

static PyObject *
partial_sizeof(PyIUObject_Partial *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t res;
    res = sizeof(PyIUObject_Partial);
    /* Include the size of the posph array. */
    res += self->numph * sizeof(Py_ssize_t);
    return PyLong_FromSsize_t(res);
}

static PyMethodDef partial_methods[] = {
    {
        "__reduce__",                /* ml_name */
        (PyCFunction)partial_reduce, /* ml_meth */
        METH_NOARGS,                 /* ml_flags */
        PYIU_reduce_doc              /* ml_doc */
    },
    {
        "__setstate__",                /* ml_name */
        (PyCFunction)partial_setstate, /* ml_meth */
        METH_O,                        /* ml_flags */
        PYIU_setstate_doc              /* ml_doc */
    },
    {
        "__sizeof__",                /* ml_name */
        (PyCFunction)partial_sizeof, /* ml_meth */
        METH_NOARGS,                 /* ml_flags */
        PYIU_sizeof_doc              /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

#if PYIU_PYPY

static PyGetSetDef partial_getsetlist[] = {
    {
        "__dict__",                /* name */
        (getter)partial_get_dict,  /* get */
        (setter)partial_set_dict,  /* set */
        partial_prop___dict___doc, /* doc */
        (void *)NULL               /* closure */
    },
    {NULL} /* sentinel */
};

#else

static PyGetSetDef partial_getsetlist[] = {
    {
        "__dict__",                /* name */
        PyObject_GenericGetDict,   /* get */
        PyObject_GenericSetDict,   /* set */
        partial_prop___dict___doc, /* doc */
        (void *)NULL               /* closure */
    },
    {NULL} /* sentinel */
};

#endif

static PyMemberDef partial_memberlist[] = {
    {
        "func",                           /* name */
        T_OBJECT,                         /* type */
        offsetof(PyIUObject_Partial, fn), /* offset */
        READONLY,                         /* flags */
        partial_prop_func_doc             /* doc */
    },
    {
        "args",                             /* name */
        T_OBJECT,                           /* type */
        offsetof(PyIUObject_Partial, args), /* offset */
        READONLY,                           /* flags */
        partial_prop_args_doc               /* doc */
    },
    {
        "keywords",                       /* name */
        T_OBJECT,                         /* type */
        offsetof(PyIUObject_Partial, kw), /* offset */
        READONLY,                         /* flags */
        partial_prop_keywords_doc         /* doc */
    },
    {
        "num_placeholders",                  /* name */
        T_PYSSIZET,                          /* type */
        offsetof(PyIUObject_Partial, numph), /* offset */
        READONLY,                            /* flags */
        partial_prop_nplaceholders_doc       /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Partial = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.partial", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Partial),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                              /* tp_itemsize */
    /* methods */
    (destructor)partial_dealloc, /* tp_dealloc */
#if PyIU_USE_VECTORCALL
    offsetof(PyIUObject_Partial, vectorcall), /* tp_vectorcall_offset */
#else
    (printfunc)0,              /* tp_print */
#endif
    (getattrfunc)0,         /* tp_getattr */
    (setattrfunc)0,         /* tp_setattr */
    0,                      /* tp_reserved */
    (reprfunc)partial_repr, /* tp_repr */
    (PyNumberMethods *)0,   /* tp_as_number */
    (PySequenceMethods *)0, /* tp_as_sequence */
    (PyMappingMethods *)0,  /* tp_as_mapping */
    (hashfunc)0,            /* tp_hash */
#if PyIU_USE_VECTORCALL
    (ternaryfunc)PyVectorcall_Call, /* tp_call */
#else
    (ternaryfunc)partial_call, /* tp_call */
#endif
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)PyObject_GenericSetAttr, /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE
#if PyIU_USE_VECTORCALL
    #if PyIU_USE_UNDERSCORE_VECTORCALL
        | _Py_TPFLAGS_HAVE_VECTORCALL
    #else
        | Py_TPFLAGS_HAVE_VECTORCALL
    #endif
#endif
    ,                                                      /* tp_flags */
    (const char *)partial_doc,                             /* tp_doc */
    (traverseproc)partial_traverse,                        /* tp_traverse */
    (inquiry)partial_clear,                                /* tp_clear */
    (richcmpfunc)0,                                        /* tp_richcompare */
    (Py_ssize_t)offsetof(PyIUObject_Partial, weakreflist), /* tp_weaklistoffset */
    (getiterfunc)0,                                        /* tp_iter */
    (iternextfunc)0,                                       /* tp_iternext */
    partial_methods,                                       /* tp_methods */
    partial_memberlist,                                    /* tp_members */
    partial_getsetlist,                                    /* tp_getset */
    0,                                                     /* tp_base */
    0,                                                     /* tp_dict */
    (descrgetfunc)0,                                       /* tp_descr_get */
    (descrsetfunc)0,                                       /* tp_descr_set */
    (Py_ssize_t)offsetof(PyIUObject_Partial, dict),        /* tp_dictoffset */
    (initproc)0,                                           /* tp_init */
    (allocfunc)0,                                          /* tp_alloc */
    (newfunc)partial_new,                                  /* tp_new */
    (freefunc)PyObject_GC_Del,                             /* tp_free */
};

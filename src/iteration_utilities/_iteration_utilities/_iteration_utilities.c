/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "docsfunctions.h"
#include "helper.h"

#include "always_iterable.h"

#include "exported_helper.h"
#include "isx.h"
#include "itemidxkey.h"
#include "mathematical.h"
#include "placeholder.h"
#include "returnx.h"
#include "seen.h"

#include "chained.h"
#include "complement.h"
#include "constant.h"
#include "flip.h"
#include "packed.h"
#include "partial.h"

#include "nth.h"

#include "alldistinct.h"
#include "allequal.h"
#include "allisinstance.h"
#include "allmonotone.h"
#include "anyisinstance.h"
#include "argminmax.h"
#include "countitems.h"
#include "dotproduct.h"
#include "groupedby.h"
#include "minmax.h"
#include "one.h"
#include "partition.h"

#include "accumulate.h"
#include "applyfunc.h"
#include "clamp.h"
#include "deepflatten.h"
#include "duplicates.h"
#include "empty.h"
#include "grouper.h"
#include "intersperse.h"
#include "iterexcept.h"
#include "merge.h"
#include "replicate.h"
#include "roundrobin.h"
#include "sideeffect.h"
#include "split.h"
#include "starfilter.h"
#include "successive.h"
#include "tabulate.h"
#include "uniqueever.h"
#include "uniquejust.h"

static PyMethodDef PyIU_methods[] = {
    /* isx */
    {
        "is_None",                /* ml_name */
        (PyCFunction)PyIU_IsNone, /* ml_meth */
        METH_O,                   /* ml_flags */
        PyIU_IsNone_doc           /* ml_doc */
    },
    {
        "is_not_None",               /* ml_name */
        (PyCFunction)PyIU_IsNotNone, /* ml_meth */
        METH_O,                      /* ml_flags */
        PyIU_IsNotNone_doc           /* ml_doc */
    },
    {
        "is_even",                /* ml_name */
        (PyCFunction)PyIU_IsEven, /* ml_meth */
        METH_O,                   /* ml_flags */
        PyIU_IsEven_doc           /* ml_doc */
    },
    {
        "is_odd",                /* ml_name */
        (PyCFunction)PyIU_IsOdd, /* ml_meth */
        METH_O,                  /* ml_flags */
        PyIU_IsOdd_doc           /* ml_doc */
    },
    {
        "is_iterable",                /* ml_name */
        (PyCFunction)PyIU_IsIterable, /* ml_meth */
        METH_O,                       /* ml_flags */
        PyIU_IsIterable_doc           /* ml_doc */
    },
    /* Math */
    {
        "square",                     /* ml_name */
        (PyCFunction)PyIU_MathSquare, /* ml_meth */
        METH_O,                       /* ml_flags */
        PyIU_MathSquare_doc           /* ml_doc */
    },
    {
        "double",                     /* ml_name */
        (PyCFunction)PyIU_MathDouble, /* ml_meth */
        METH_O,                       /* ml_flags */
        PyIU_MathDouble_doc           /* ml_doc */
    },
    {
        "reciprocal",                     /* ml_name */
        (PyCFunction)PyIU_MathReciprocal, /* ml_meth */
        METH_O,                           /* ml_flags */
        PyIU_MathReciprocal_doc           /* ml_doc */
    },
    {
        "radd",                     /* ml_name */
        (PyCFunction)PyIU_MathRadd, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRadd_doc           /* ml_doc */
    },
    {
        "rsub",                     /* ml_name */
        (PyCFunction)PyIU_MathRsub, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRsub_doc           /* ml_doc */
    },
    {
        "rmul",                     /* ml_name */
        (PyCFunction)PyIU_MathRmul, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRmul_doc           /* ml_doc */
    },
    {
        "rdiv",                     /* ml_name */
        (PyCFunction)PyIU_MathRdiv, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRdiv_doc           /* ml_doc */
    },
    {
        "rfdiv",                     /* ml_name */
        (PyCFunction)PyIU_MathRfdiv, /* ml_meth */
        METH_VARARGS,                /* ml_flags */
        PyIU_MathRfdiv_doc           /* ml_doc */
    },
    {
        "rpow",                     /* ml_name */
        (PyCFunction)PyIU_MathRpow, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRpow_doc           /* ml_doc */
    },
    {
        "rmod",                     /* ml_name */
        (PyCFunction)PyIU_MathRmod, /* ml_meth */
        METH_VARARGS,               /* ml_flags */
        PyIU_MathRmod_doc           /* ml_doc */
    },
    /* Helper */
    {
        "_parse_args", /* ml_name */
#if PyIU_USE_VECTORCALL
        (PyCFunction)(void (*)(void))PyIU_TupleToList_and_InsertItemAtIndex, /* ml_meth */
        METH_FASTCALL,                                                       /* ml_flags */
#else
        (PyCFunction)PyIU_TupleToList_and_InsertItemAtIndex, /* ml_meth */
        METH_VARARGS,                                        /* ml_flags */
#endif
        PyIU_TupleToList_and_InsertItemAtIndex_doc /* ml_doc */
    },
    {
        "_parse_kwargs", /* ml_name */
#if PyIU_USE_VECTORCALL
        (PyCFunction)(void (*)(void))PyIU_RemoveFromDictWhereValueIs, /* ml_meth */
        METH_FASTCALL,                                                /* ml_flags */
#else
        (PyCFunction)PyIU_RemoveFromDictWhereValueIs,        /* ml_meth */
        METH_VARARGS,                                        /* ml_flags */
#endif
        PyIU_RemoveFromDictWhereValueIs_doc /* ml_doc */
    },
    /* returnx */
    {
        "return_identity",                /* ml_name */
        (PyCFunction)PyIU_ReturnIdentity, /* ml_meth */
        METH_O,                           /* ml_flags */
        PyIU_ReturnIdentity_doc           /* ml_doc */
    },
    {
        "return_first_arg",               /* ml_name */
        (PyCFunction)PyIU_ReturnFirstArg, /* ml_meth */
        METH_VARARGS | METH_KEYWORDS,     /* ml_flags */
        PyIU_ReturnFirstArg_doc           /* ml_doc */
    },
    {
        "return_called",                /* ml_name */
        (PyCFunction)PyIU_ReturnCalled, /* ml_meth */
        METH_O,                         /* ml_flags */
        PyIU_ReturnCalled_doc           /* ml_doc */
    },
    {
        "always_iterable",                /* ml_name */
        (PyCFunction)PyIU_AlwaysIterable, /* ml_meth */
        METH_VARARGS | METH_KEYWORDS,     /* ml_flags */
        PyIU_AlwaysIterable_doc           /* ml_doc */
    },
    /* Fold functions */
    {
        "argmin",                     /* ml_name */
        (PyCFunction)PyIU_Argmin,     /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Argmin_doc               /* ml_doc */
    },
    {
        "argmax",                     /* ml_name */
        (PyCFunction)PyIU_Argmax,     /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Argmax_doc               /* ml_doc */
    },
    {
        "all_distinct",                /* ml_name */
        (PyCFunction)PyIU_AllDistinct, /* ml_meth */
        METH_O,                        /* ml_flags */
        PyIU_AllDistinct_doc           /* ml_doc */
    },
    {
        "all_equal",                /* ml_name */
        (PyCFunction)PyIU_AllEqual, /* ml_meth */
        METH_O,                     /* ml_flags */
        PyIU_AllEqual_doc           /* ml_doc */
    },
    {
        "all_isinstance",                /* ml_name */
        (PyCFunction)PyIU_AllIsinstance, /* ml_meth */
        METH_VARARGS | METH_KEYWORDS,    /* ml_flags */
        PyIU_AllIsinstance_doc           /* ml_doc */
    },
    {
        "all_monotone",               /* ml_name */
        (PyCFunction)PyIU_Monotone,   /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Monotone_doc             /* ml_doc */
    },
    {
        "any_isinstance",                /* ml_name */
        (PyCFunction)PyIU_AnyIsinstance, /* ml_meth */
        METH_VARARGS | METH_KEYWORDS,    /* ml_flags */
        PyIU_AnyIsinstance_doc           /* ml_doc */
    },
    {
        "count_items",                /* ml_name */
        (PyCFunction)PyIU_Count,      /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Count_doc                /* ml_doc */
    },
    {
        "dotproduct",                 /* ml_name */
        (PyCFunction)PyIU_DotProduct, /* ml_meth */
        METH_VARARGS,                 /* ml_flags */
        PyIU_DotProduct_doc           /* ml_doc */
    },
    {
        "groupedby",                  /* ml_name */
        (PyCFunction)PyIU_Groupby,    /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Groupby_doc              /* ml_doc */
    },
    {
        "minmax",                     /* ml_name */
        (PyCFunction)PyIU_MinMax,     /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_MinMax_doc               /* ml_doc */
    },
    {
        "one",                 /* ml_name */
        (PyCFunction)PyIU_One, /* ml_meth */
        METH_O,                /* ml_flags */
        PyIU_One_doc           /* ml_doc */
    },
    {
        "partition",                  /* ml_name */
        (PyCFunction)PyIU_Partition,  /* ml_meth */
        METH_VARARGS | METH_KEYWORDS, /* ml_flags */
        PyIU_Partition_doc            /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static int _iteration_utilities_exec(PyObject *module) {
    /* Classes available in module. */
    PyTypeObject *typelist[] = {
        &PyIUType_ItemIdxKey,
        &PyIUType_Seen,

        &PyIUType_Chained,
        &PyIUType_Complement,
        &PyIUType_Constant,
        &PyIUType_Flip,
        &PyIUType_Packed,

        &Placeholder_Type,
        &PyIUType_Partial,

        &PyIUType_Nth,

        &PyIUType_Accumulate,
        &PyIUType_Applyfunc,
        &PyIUType_Clamp,
        &PyIUType_DeepFlatten,
        &PyIUType_Duplicates,
        &PyIUType_Empty,
        &PyIUType_Grouper,
        &PyIUType_Intersperse,
        &PyIUType_Iterexcept,
        &PyIUType_Merge,
        &PyIUType_Replicate,
        &PyIUType_Roundrobin,
        &PyIUType_Sideeffects,
        &PyIUType_Split,
        &PyIUType_Starfilter,
        &PyIUType_Successive,
        &PyIUType_Tabulate,
        &PyIUType_UniqueEver,
        &PyIUType_UniqueJust,
        NULL };

    size_t i;
    /* Add classes to the module but only use the name starting after the first
        occurrence of ".".
        */
    for (i = 0; typelist[i] != NULL; i++) {
        #if PyIU_USE_BUILTIN_MODULE_ADDTYPE
            if (PyModule_AddType(module, typelist[i]) < 0) {
                return -1;
            }
        #else
            char *name;
            if (PyType_Ready(typelist[i]) < 0) {
                return -1;
            }
            name = strrchr(typelist[i]->tp_name, '.');
            assert(name != NULL);
            Py_INCREF(typelist[i]);
            if (PyModule_AddObject(module, name + 1, (PyObject *)typelist[i]) < 0) {
                return -1;
            }
        #endif
    }
    Py_INCREF(PYIU_Placeholder);
    if (PyModule_AddObject(module, PyIU_Placeholder_name, PYIU_Placeholder) < 0) {
        return -1;
    }
    Py_INCREF(PYIU_Empty);
    if (PyModule_AddObject(module, PyIU_Empty_name, PYIU_Empty) < 0) {
        return -1;
    }
    if (PyDict_SetItemString(PyIUType_Partial.tp_dict, "_", PYIU_Placeholder) < 0) {
        return -1;
    }
    return 0;
}

static PyModuleDef_Slot _iteration_utilities_slots[] = {
    {Py_mod_exec, _iteration_utilities_exec},
    {0, NULL}
};

/* Module definition */
static struct PyModuleDef PyIU_module = {
    PyModuleDef_HEAD_INIT,                                 /* m_base */
    PyIU_module_name,                                      /* m_name */
    PyIU_module_doc,                                       /* m_doc */
    0,                                                     /* m_size */
    (PyMethodDef *)PyIU_methods,                           /* m_methods */
    (struct PyModuleDef_Slot*)_iteration_utilities_slots,  /* m_slots */
    NULL,                                                  /* m_traverse */
    NULL,                                                  /* m_clear */
    NULL                                                   /* m_free */
};

/* Module initialization */
PyMODINIT_FUNC
PyInit__iteration_utilities(void) {
    PyIU_InitializeConstants();
    return PyModuleDef_Init(&PyIU_module);
}

/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "docsfunctions.h"
#include "helper.h"

#include "exported_helper.h"
#include "seen.h"
#include "itemidxkey.h"
#include "isx.h"
#include "returnx.h"
#include "placeholder.h"
#include "mathematical.h"

#include "chained.h"
#include "complement.h"
#include "constant.h"
#include "flip.h"
#include "packed.h"
#include "partial.h"

#include "nth.h"

#include "argminmax.h"
#include "alldistinct.h"
#include "allequal.h"
#include "allisinstance.h"
#include "allmonotone.h"
#include "anyisinstance.h"
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

    {"is_None",                                         /* ml_name */
     (PyCFunction)PyIU_IsNone,                          /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_IsNone_doc                                    /* ml_doc */
     },

    {"is_not_None",                                     /* ml_name */
     (PyCFunction)PyIU_IsNotNone,                       /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_IsNotNone_doc                                 /* ml_doc */
     },

    {"is_even",                                         /* ml_name */
     (PyCFunction)PyIU_IsEven,                          /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_IsEven_doc                                    /* ml_doc */
     },

    {"is_odd",                                          /* ml_name */
     (PyCFunction)PyIU_IsOdd,                           /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_IsOdd_doc                                     /* ml_doc */
     },

    {"is_iterable",                                     /* ml_name */
     (PyCFunction)PyIU_IsIterable,                      /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_IsIterable_doc                                /* ml_doc */
     },

    /* Math */

    {"square",                                          /* ml_name */
     (PyCFunction)PyIU_MathSquare,                      /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_MathSquare_doc                                /* ml_doc */
     },

    {"double",                                          /* ml_name */
     (PyCFunction)PyIU_MathDouble,                      /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_MathDouble_doc                                /* ml_doc */
     },

    {"reciprocal",                                      /* ml_name */
     (PyCFunction)PyIU_MathReciprocal,                  /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_MathReciprocal_doc                            /* ml_doc */
     },

    {"radd",                                            /* ml_name */
     (PyCFunction)PyIU_MathRadd,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRadd_doc                                  /* ml_doc */
     },

    {"rsub",                                            /* ml_name */
     (PyCFunction)PyIU_MathRsub,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRsub_doc                                  /* ml_doc */
     },

    {"rmul",                                            /* ml_name */
     (PyCFunction)PyIU_MathRmul,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRmul_doc                                  /* ml_doc */
     },

    {"rdiv",                                            /* ml_name */
     (PyCFunction)PyIU_MathRdiv,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRdiv_doc                                  /* ml_doc */
     },

    {"rfdiv",                                           /* ml_name */
     (PyCFunction)PyIU_MathRfdiv,                       /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRfdiv_doc                                 /* ml_doc */
     },

    {"rpow",                                            /* ml_name */
     (PyCFunction)PyIU_MathRpow,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRpow_doc                                  /* ml_doc */
     },

    {"rmod",                                            /* ml_name */
     (PyCFunction)PyIU_MathRmod,                        /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_MathRmod_doc                                  /* ml_doc */
     },

    /* Helper */

    {"_parse_args",                                     /* ml_name */
#if PyIU_USE_VECTORCALL
     (PyCFunction)(void(*)(void))PyIU_TupleToList_and_InsertItemAtIndex, /* ml_meth */
     METH_FASTCALL,                                     /* ml_flags */
#else
     (PyCFunction)PyIU_TupleToList_and_InsertItemAtIndex, /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
#endif
     PyIU_TupleToList_and_InsertItemAtIndex_doc         /* ml_doc */
     },

    {"_parse_kwargs",                                   /* ml_name */
#if PyIU_USE_VECTORCALL
     (PyCFunction)(void(*)(void))PyIU_RemoveFromDictWhereValueIs, /* ml_meth */
     METH_FASTCALL,                                     /* ml_flags */
#else
     (PyCFunction)PyIU_RemoveFromDictWhereValueIs,      /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
#endif
     PyIU_RemoveFromDictWhereValueIs_doc                /* ml_doc */
     },

    /* returnx */

    {"return_identity",                                 /* ml_name */
     (PyCFunction)PyIU_ReturnIdentity,                  /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_ReturnIdentity_doc                            /* ml_doc */
     },

    {"return_first_arg",                                /* ml_name */
     (PyCFunction)PyIU_ReturnFirstArg,                  /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_ReturnFirstArg_doc                            /* ml_doc */
     },

    {"return_called",                                   /* ml_name */
     (PyCFunction)PyIU_ReturnCalled,                    /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_ReturnCalled_doc                              /* ml_doc */
     },

    /* Fold functions */

    {"argmin",                                          /* ml_name */
     (PyCFunction)PyIU_Argmin,                          /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Argmin_doc                                    /* ml_doc */
     },

    {"argmax",                                          /* ml_name */
     (PyCFunction)PyIU_Argmax,                          /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Argmax_doc                                    /* ml_doc */
     },

    {"all_distinct",                                    /* ml_name */
     (PyCFunction)PyIU_AllDistinct,                     /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_AllDistinct_doc                               /* ml_doc */
     },

    {"all_equal",                                       /* ml_name */
     (PyCFunction)PyIU_AllEqual,                        /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_AllEqual_doc                                  /* ml_doc */
     },

    {"all_isinstance",                                  /* ml_name */
     (PyCFunction)PyIU_AllIsinstance,                   /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_AllIsinstance_doc                             /* ml_doc */
     },

    {"all_monotone",                                    /* ml_name */
     (PyCFunction)PyIU_Monotone,                        /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Monotone_doc                                  /* ml_doc */
     },

    {"any_isinstance",                                  /* ml_name */
     (PyCFunction)PyIU_AnyIsinstance,                   /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_AnyIsinstance_doc                             /* ml_doc */
     },

    {"count_items",                                     /* ml_name */
     (PyCFunction)PyIU_Count,                           /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Count_doc                                     /* ml_doc */
     },

    {"dotproduct",                                      /* ml_name */
     (PyCFunction)PyIU_DotProduct,                      /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_DotProduct_doc                                /* ml_doc */
     },

    {"groupedby",                                       /* ml_name */
     (PyCFunction)PyIU_Groupby,                         /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Groupby_doc                                   /* ml_doc */
     },

    {"minmax",                                          /* ml_name */
     (PyCFunction)PyIU_MinMax,                          /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_MinMax_doc                                    /* ml_doc */
     },

    {"one",                                             /* ml_name */
     (PyCFunction)PyIU_One,                             /* ml_meth */
     METH_O,                                            /* ml_flags */
     PyIU_One_doc                                       /* ml_doc */
     },

    {"partition",                                       /* ml_name */
     (PyCFunction)PyIU_Partition,                       /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Partition_doc                                 /* ml_doc */
     },

    {NULL, NULL}
};

/* Module definition */
static struct PyModuleDef PyIU_module = {
    PyModuleDef_HEAD_INIT,                            /* m_base */
    PyIU_module_name,                                 /* m_name */
    PyIU_module_doc,                                  /* m_doc */
    (Py_ssize_t)-1,                                   /* m_size */
    (PyMethodDef *)PyIU_methods,                      /* m_methods */
    NULL,                                             /* m_slots or m_reload */
    (traverseproc)NULL,                               /* m_traverse */
    (inquiry)NULL,                                    /* m_clear */
    (freefunc)NULL                                    /* m_free */
};

/* Module initialization */
PyMODINIT_FUNC
PyInit__iteration_utilities(void)
{
    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;
    PyObject *PyIU_returnTrue, *PyIU_returnFalse, *PyIU_returnNone;
    PyObject *PyIU_ReduceFirst, *PyIU_ReduceSecond, *PyIU_ReduceThird, *PyIU_ReduceLast;
    Py_ssize_t minus_one = -1;  /* no idea why this is needed but -1 in call doesn't work */

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
        NULL
    };

    m = PyModule_Create(&PyIU_module);


    if (m != NULL) {

        /* Add classes to the module but only use the name starting after the first
           occurrence of ".".
           */
        for (i=0 ; typelist[i] != NULL ; i++) {
            if (PyType_Ready(typelist[i]) < 0)
                return m;
            name = strchr(typelist[i]->tp_name, '.');
            assert (name != NULL);
            Py_INCREF(typelist[i]);
            PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
        }
        Py_INCREF(PYIU_Placeholder);
        PyModule_AddObject(m, PyIU_Placeholder_name, PYIU_Placeholder);

        if (PyDict_SetItemString(PyIUType_Partial.tp_dict, "_", PYIU_Placeholder) != 0) {
            return m;
        }

        /* Add pre-defined instances. */
        PyIU_InitializeConstants();
        PyIU_returnTrue = PyIUConstant_New(Py_True);
        PyModule_AddObject(m, PyIU_returnTrue_name, PyIU_returnTrue);
        PyIU_returnFalse = PyIUConstant_New(Py_False);
        PyModule_AddObject(m, PyIU_returnFalse_name, PyIU_returnFalse);
        PyIU_returnNone = PyIUConstant_New(Py_None);
        PyModule_AddObject(m, PyIU_returnNone_name, PyIU_returnNone);

        PyIU_ReduceFirst = PyIUNth_New(0);
        PyModule_AddObject(m, PyIU_ReduceFirst_name, PyIU_ReduceFirst);
        PyIU_ReduceSecond = PyIUNth_New(1);
        PyModule_AddObject(m, PyIU_ReduceSecond_name, PyIU_ReduceSecond);
        PyIU_ReduceThird = PyIUNth_New(2);
        PyModule_AddObject(m, PyIU_ReduceThird_name, PyIU_ReduceThird);
        PyIU_ReduceLast = PyIUNth_New(minus_one);
        PyModule_AddObject(m, PyIU_ReduceLast_name, PyIU_ReduceLast);
    }

    return m;
}

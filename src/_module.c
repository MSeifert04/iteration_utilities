/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include <Python.h>
#include <structmember.h>

#include "_globaldocs.c"

#include "_exported_helper.c"
#include "_helper.c"
#include "_seen.c"
#include "_itemidxkey.c"
#include "_isx.c"
#include "_returnx.c"
#include "_placeholder.c"
#include "_mathematical.c"

#include "chained.c"
#include "complement.c"
#include "constant.c"
#include "flip.c"
#include "packed.c"
#include "partial.c"

#include "nth.c"

#include "argminmax.c"
#include "alldistinct.c"
#include "allequal.c"
#include "allmonotone.c"
#include "countitems.c"
#include "dotproduct.c"
#include "groupedby.c"
#include "minmax.c"
#include "one.c"
#include "partition.c"

#include "accumulate.c"
#include "applyfunc.c"
#include "clamp.c"
#include "deepflatten.c"
#include "duplicates.c"
#include "grouper.c"
#include "intersperse.c"
#include "iterexcept.c"
#include "merge.c"
#include "roundrobin.c"
#include "sideeffect.c"
#include "split.c"
#include "starfilter.c"
#include "successive.c"
#include "tabulate.c"
#include "uniqueever.c"
#include "uniquejust.c"


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
     (PyCFunction)PyIU_TupleToList_and_InsertItemAtIndex, /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
     PyIU_TupleToList_and_InsertItemAtIndex_doc         /* ml_doc */
     },

    {"_parse_kwargs",                                   /* ml_name */
     (PyCFunction)PyIU_RemoveFromDictWhereValueIs,      /* ml_meth */
     METH_VARARGS,                                      /* ml_flags */
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

    {"all_monotone",                                    /* ml_name */
     (PyCFunction)PyIU_Monotone,                        /* ml_meth */
     METH_VARARGS | METH_KEYWORDS,                      /* ml_flags */
     PyIU_Monotone_doc                                  /* ml_doc */
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

#if PY_MAJOR_VERSION >= 3
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
  PyInit__cfuncs(void)
  {
#else
  void
  init_cfuncs(void)
  {
#endif

    //Py_Initialize();
    int i;
    PyObject *m;
    char *name;
    PyObject *PyIU_returnTrue, *PyIU_returnFalse, *PyIU_returnNone;
    PyObject *PyIU_ReduceFirst, *PyIU_ReduceSecond, *PyIU_ReduceThird, *PyIU_ReduceLast;
    Py_ssize_t minus_one = -1;  /* no idea why this is needed but -1 in call doesn't work */

    /* Classes avaiable in module. */
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

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&PyIU_module);
#else
    m = Py_InitModule3(PyIU_module_name, PyIU_methods, PyIU_module_doc);
#endif

    if (m != NULL) {

        /* Add classes to the module but only use the name starting after the first
           occurence of ".".
           */
        for (i=0 ; typelist[i] != NULL ; i++) {
            if (PyType_Ready(typelist[i]) < 0)
#if PY_MAJOR_VERSION >= 3
                return m;
#else
                return;
#endif
            name = strchr(typelist[i]->tp_name, '.');
            assert (name != NULL);
            Py_INCREF(typelist[i]);
            PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
        }
        Py_INCREF(PYIU_Placeholder);
        PyModule_AddObject(m, PyIU_Placeholder_name, PYIU_Placeholder);

        if (PyDict_SetItemString(PyIUType_Partial.tp_dict, "_", PYIU_Placeholder) != 0) {
#if PY_MAJOR_VERSION >= 3
            return m;
#else
            return;
#endif
        }

        /* Add pre-defined instances. */
        PyIU_returnTrue = constant_new(&PyIUType_Constant,
                                       Py_BuildValue("(O)", Py_True),
                                       NULL);
        PyModule_AddObject(m, PyIU_returnTrue_name, PyIU_returnTrue);
        PyIU_returnFalse = constant_new(&PyIUType_Constant,
                                        Py_BuildValue("(O)", Py_False),
                                        NULL);
        PyModule_AddObject(m, PyIU_returnFalse_name, PyIU_returnFalse);
        PyIU_returnNone = constant_new(&PyIUType_Constant,
                                       Py_BuildValue("(O)", Py_None),
                                       NULL);
        PyModule_AddObject(m, PyIU_returnNone_name, PyIU_returnNone);

        PyIU_ReduceFirst = nth_new(&PyIUType_Nth,
                                   Py_BuildValue("(n)", 0),
                                   NULL);
        PyModule_AddObject(m, PyIU_ReduceFirst_name, PyIU_ReduceFirst);
        PyIU_ReduceSecond = nth_new(&PyIUType_Nth,
                                    Py_BuildValue("(n)", 1),
                                    NULL);
        PyModule_AddObject(m, PyIU_ReduceSecond_name, PyIU_ReduceSecond);
        PyIU_ReduceThird = nth_new(&PyIUType_Nth,
                                   Py_BuildValue("(n)", 2),
                                   NULL);
        PyModule_AddObject(m, PyIU_ReduceThird_name, PyIU_ReduceThird);
        PyIU_ReduceLast = nth_new(&PyIUType_Nth,
                                  Py_BuildValue("(n)", minus_one),
                                  NULL);
        PyModule_AddObject(m, PyIU_ReduceLast_name, PyIU_ReduceLast);

        #if PY_MAJOR_VERSION == 2
            PyIU_global_zero = PyInt_FromLong((long)0);
            PyIU_global_one = PyInt_FromLong((long)1);
            PyIU_global_two = PyInt_FromLong((long)2);
        #else
            PyIU_global_zero = PyLong_FromLong((long)0);
            PyIU_global_one = PyLong_FromLong((long)1);
            PyIU_global_two = PyLong_FromLong((long)2);
        #endif
        PyIU_global_0tuple = PyTuple_New(0);
    }

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}

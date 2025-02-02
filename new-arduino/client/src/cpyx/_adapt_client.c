#include <adapt/client/listener.h>

// Standard boilerplate for CPython extension modules.
// NOTE: this is not necessary since Python 3.13.
#define PY_SSIZE_T_CLEAN

// NOTE: `Python.h` should be included before standard library headers.
#include <Python.h>

#include <stddef.h>

typedef struct {
  // clang-format off
  PyObject_HEAD
  adptc_listener lsnr;
  bool is_lsning;
  // clang-format on
} ListenerObject;

static PyObject *Listener_new(PyTypeObject *const up_type,
                              [[maybe_unused]] PyObject *const up_args,
                              [[maybe_unused]] PyObject *const up_kwds) {
  ListenerObject *const self = (ListenerObject *)up_type->tp_alloc(up_type, 0);
  if (!self)
    return NULL;

  self->lsnr = adptc_listener_create();
  if (!self->lsnr) {
    Py_DECREF(self);
    return NULL;
  }

  self->is_lsning = false;

  return (PyObject *)self;
}

static void Listener_dealloc(PyObject *const up_self) {
  ListenerObject *const self = (ListenerObject *)up_self;

  adptc_listener_destroy(self->lsnr);

  Py_TYPE(self)->tp_free(up_self);
}

static void listener_routine(adptc_listener_receiver const rcvr,
                             void *const user_ctx) {}

static int Listener_init(PyObject *const up_self, PyObject *const up_args,
                         [[maybe_unused]] PyObject *const up_kwds) {
  ListenerObject *const self = (ListenerObject *)up_self;

  PyObject *up_serial_file = NULL;
  PyObject *up_user_fn = NULL;
  if (!PyArg_ParseTuple(up_args, "OO!", &up_serial_file, &PyFunction_Type,
                        &up_user_fn))
    return -1;

  int const serial_fd = PyObject_AsFileDescriptor(up_serial_file);
  if (serial_fd < 0) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be file");
    return -1;
  }

  PyObject *const up_user_fn_closure = PyFunction_GetClosure(up_user_fn);

  if (!self->is_lsning)
    adptc_listener_start(self->lsnr, serial_fd, listener_routine, NULL);

  self->is_lsning = true;

  return 0;
}

static PyObject *Listener_stop(PyObject *const up_self,
                               [[maybe_unused]] PyObject *const up_args) {
  ListenerObject *const self = (ListenerObject *)up_self;
  if (self->is_lsning)
    adptc_listener_stop(self->lsnr);

  self->is_lsning = false;

  Py_RETURN_NONE;
}

static PyMethodDef Listener_methods[] = {{.ml_name = "stop",
                                          .ml_meth = Listener_stop,
                                          .ml_flags = METH_NOARGS,
                                          .ml_doc = "Stop listening"},
                                         {0}};

static PyTypeObject ListenerType = {
    // clang-format off
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_adapt_client.Listener",
    .tp_basicsize = sizeof(ListenerObject),
    .tp_itemsize = 0,
    .tp_dealloc = Listener_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = PyDoc_STR("Listener"),
    .tp_methods = Listener_methods,
    .tp_init = Listener_init,
    .tp_new = Listener_new,
    // clang-format on
};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_adapt_client",
    .m_doc = NULL,
    .m_size = -1,
};

// NOTE: double underscores create a reserved identifier in C++, but not in C.
//       Best not to compile this with a C++ compiler, then.
PyMODINIT_FUNC PyInit__adapt_client(void) {
  if (PyType_Ready(&ListenerType) < 0)
    return NULL;

  PyObject *const mod = PyModule_Create(&module);
  if (!mod)
    return NULL;

  if (PyModule_AddObjectRef(mod, "Listener", (PyObject *)&ListenerType) < 0) {
    Py_DECREF(mod);
    return NULL;
  }

  return mod;
}

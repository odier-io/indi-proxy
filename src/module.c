/*--------------------------------------------------------------------------------------------------------------------*/
#define PY_SSIZE_T_CLEAN
/*--------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include <Python.h>

#include "indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    PyObject_HEAD

    indi_object_t *object;

} PyIndiObject;

static PyTypeObject PyIndiObjectType;

static PyObject *PyIndiObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    PyObject_HEAD

    indi_xmldoc_t *xmldoc;

} PyIndiXMLDoc;

static PyTypeObject PyIndiXMLDocType;

static PyObject *PyIndiXMLDoc_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    PyObject_HEAD

    indi_proxy_t proxy;

} PyIndiProxy;

static PyTypeObject PyIndiProxyType;

static PyObject *PyIndiProxy_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/*--------------------------------------------------------------------------------------------------------------------*/
/* MEMORY                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_cleanup(PyObject *self, PyObject *args)
{
    indi_memory_finalize();

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* OBJECT                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyIndiObject *self = (PyIndiObject *) type->tp_alloc(type, 0);

    if(self != NULL)
    {
        self->object = NULL;
    }

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void PyIndiObject_dealloc(PyIndiObject *self)
{
    if(self->object)
    {
        indi_object_free(self->object);
    }

    Py_TYPE(self)->tp_free((PyObject *) self);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int PyIndiObject_init(PyIndiObject *self, PyObject *args, PyObject *kwds)
{
    STR_t json;

    if(!PyArg_ParseTuple(args, "y", &json))
    {
        return -1;
    }

    self->object = indi_object_parse(json);

    if(self->object == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to parse JSON content");

        return -1;
    }

    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiObject_toBytes(PyIndiObject *self)
{
    str_t result = indi_object_to_string(self->object);
    PyObject *py_result = PyBytes_FromString(result);
    indi_memory_free(result);

    return py_result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiObject_toXMLDoc(PyIndiObject *self, PyObject *args)
{
    int validate;

    if(!PyArg_ParseTuple(args, "p", &validate))
    {
        return NULL;
    }

    indi_xmldoc_t *xmldoc = indi_object_to_xmldoc(self->object, validate);

    if(xmldoc == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert to Object to XMLDoc");

        return NULL;
    }

    PyIndiXMLDoc *py_xmldoc = (PyIndiXMLDoc *) PyIndiXMLDoc_new(&PyIndiXMLDocType, NULL, NULL);

    if(py_xmldoc == NULL)
    {
        indi_xmldoc_free(xmldoc);

        return NULL;
    }

    py_xmldoc->xmldoc = xmldoc;

    return (PyObject *) py_xmldoc;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* XMLDOC                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiXMLDoc_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyIndiXMLDoc *self = (PyIndiXMLDoc *) type->tp_alloc(type, 0);

    if(self != NULL)
    {
        self->xmldoc = NULL;
    }

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void PyIndiXMLDoc_dealloc(PyIndiXMLDoc *self)
{
    if(self->xmldoc)
    {
        indi_xmldoc_free(self->xmldoc);
    }

    Py_TYPE(self)->tp_free((PyObject *) self);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int PyIndiXMLDoc_init(PyIndiXMLDoc *self, PyObject *args, PyObject *kwds)
{
    STR_t json;

    if(!PyArg_ParseTuple(args, "y", &json))
    {
        return -1;
    }

    self->xmldoc = indi_xmldoc_parse(json);

    if(self->xmldoc == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to parse XML content");

        return -1;
    }

    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiXMLDoc_toBytes(PyIndiXMLDoc *self)
{
    str_t result = indi_xmldoc_to_string(self->xmldoc);
    PyObject *py_result = PyBytes_FromString(result);
    indi_memory_free(result);

    return py_result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiXMLDoc_toObject(PyIndiXMLDoc *self, PyObject *args)
{
    int validate;

    if(!PyArg_ParseTuple(args, "p", &validate))
    {
        return NULL;
    }

    indi_object_t *object = indi_xmldoc_to_object(self->xmldoc, validate);

    if(object == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert to object to xmldoc");

        return NULL;
    }

    PyIndiObject *py_object = (PyIndiObject *) PyIndiObject_new(&PyIndiObjectType, NULL, NULL);

    if(py_object == NULL)
    {
        indi_object_free(object);

        return NULL;
    }

    py_object->object = object;

    return (PyObject *) py_object;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PROXY                                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiProxy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    fprintf(stderr, "1)\n");

    return type->tp_alloc(type, 0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void PyIndiProxy_dealloc(PyIndiProxy *self)
{
    fprintf(stderr, "2)\n");

    if(self->proxy.message_buff != NULL
       ||
       self->proxy.residual_buff != NULL
    ) {
        indi_proxy_finalize(&self->proxy);
    }

    Py_TYPE(self)->tp_free((PyObject *) self);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void indi_proxy_emit(indi_proxy_t *proxy, size_t size, str_t buff)
{
    PyObject *py_string = PyBytes_FromStringAndSize(buff, size);

    PyObject *py_tuple = PyTuple_Pack(1, py_string);

    Py_XDECREF(PyObject_CallObject(
        (PyObject *) proxy->py,
        py_tuple
    ));

    Py_DECREF(py_tuple);
    Py_DECREF(py_string);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiProxy_init(PyIndiProxy *self, PyObject *args)
{
    fprintf(stderr, "3)\n");

    size_t message_buff_size;
    size_t residual_buff_size;
    PyObject *py_callable;

    if(!PyArg_ParseTuple(args, "nnO:", &message_buff_size, &residual_buff_size, &py_callable))
    {
        return NULL;
    }

    if(!PyCallable_Check(py_callable))
    {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable");

        return NULL;
    }

    indi_proxy_initialize(
        &self->proxy,
        message_buff_size,
        residual_buff_size,
        indi_proxy_emit
    );

    self->proxy.py = (buff_t) py_callable;

    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *PyIndiProxy_consume(PyIndiProxy *self, PyObject *args)
{
    fprintf(stderr, "4)\n");

    STR_t buff;
    size_t size;

    if(!PyArg_ParseTuple(args, "y#", &buff, &size))
    {
        return NULL;
    }

    indi_proxy_consume(&self->proxy, (size_t) size, (STR_t) buff);

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef PyIndiObject_methods[] = {
    {"toXMLDoc", (PyCFunction) PyIndiObject_toXMLDoc, METH_VARARGS, "???"},
    {"toBytes", (PyCFunction) PyIndiObject_toBytes, METH_VARARGS, "???"},
    /**/
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject PyIndiObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "indi_proxy.IndiObject",
    .tp_doc = "???",
    .tp_basicsize = sizeof(PyIndiObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = (newfunc) PyIndiObject_new,
    .tp_init = (initproc) PyIndiObject_init,
    .tp_dealloc = (destructor) PyIndiObject_dealloc,
    .tp_methods = PyIndiObject_methods,
};

/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef PyIndiXMLDoc_methods[] = {
    {"toObject", (PyCFunction) PyIndiXMLDoc_toObject, METH_VARARGS, "???"},
    {"toBytes", (PyCFunction) PyIndiXMLDoc_toBytes, METH_VARARGS, "???"},
    /**/
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject PyIndiXMLDocType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "indi_proxy.IndiXMLDoc",
    .tp_doc = "???",
    .tp_basicsize = sizeof(PyIndiXMLDoc),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = (newfunc) PyIndiXMLDoc_new,
    .tp_init = (initproc) PyIndiXMLDoc_init,
    .tp_dealloc = (destructor) PyIndiXMLDoc_dealloc,
    .tp_methods = PyIndiXMLDoc_methods,
};

/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef PyIndiProxy_methods[] = {
    {"consume", (PyCFunction) PyIndiProxy_consume, METH_VARARGS, "???"},
    /**/
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject PyIndiProxyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "indi_proxy.IndiProxy",
    .tp_doc = "???",
    .tp_basicsize = sizeof(PyIndiProxy),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = (newfunc) PyIndiProxy_new,
    .tp_init = (initproc) PyIndiProxy_init,
    .tp_dealloc = (destructor) PyIndiProxy_dealloc,
    .tp_methods = PyIndiProxy_methods,
};

/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef indi_proxy_methods[] = {
    {"cleanup", py_indi_cleanup, METH_VARARGS, "???"},
    /**/
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/*--------------------------------------------------------------------------------------------------------------------*/

static struct PyModuleDef indi_proxy_module = {
    PyModuleDef_HEAD_INIT, "indi_proxy", NULL, -1, indi_proxy_methods
};

/*--------------------------------------------------------------------------------------------------------------------*/

PyMODINIT_FUNC PyInit_indi_proxy(void)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_memory_initialize();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(PyType_Ready(&PyIndiObjectType) < 0
       ||
       PyType_Ready(&PyIndiXMLDocType) < 0
       ||
       PyType_Ready(&PyIndiProxyType) < 0
    ) {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    PyObject *module = PyModule_Create(&indi_proxy_module);

    if(module == NULL)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    Py_INCREF(&PyIndiObjectType);

    if(PyModule_AddObject(module, "IndiObject", (PyObject *) &PyIndiObjectType) < 0)
    {
        Py_DECREF(&PyIndiObjectType);
        Py_DECREF(module);

        return NULL;
    }

    Py_INCREF(&PyIndiXMLDocType);

    if(PyModule_AddObject(module, "IndiXMLDoc", (PyObject *) &PyIndiXMLDocType) < 0)
    {
        Py_DECREF(&PyIndiXMLDocType);
        Py_DECREF(module);

        return NULL;
    }

    Py_INCREF(&PyIndiProxyType);

    if(PyModule_AddObject(module, "IndiProxy", (PyObject *) &PyIndiProxyType) < 0)
    {
        Py_DECREF(&PyIndiProxyType);
        Py_DECREF(module);

        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return module;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

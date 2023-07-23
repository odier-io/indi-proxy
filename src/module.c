/*--------------------------------------------------------------------------------------------------------------------*/
#define PY_SSIZE_T_CLEAN
/*--------------------------------------------------------------------------------------------------------------------*/

#include <Python.h>

#include "indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/* MEMORY                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_memory_initialize(PyObject *self, PyObject *args)
{
    indi_memory_initialize();

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_memory_finalize(PyObject *self, PyObject *args)
{
    indi_memory_finalize();

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_memory_free(PyObject *self, PyObject *args)
{
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "O", &py_object))
    {
        return NULL;
    }

    buff_t object = PyLong_AsVoidPtr(py_object);

    size_t result = indi_memory_free(object);

    return PyLong_FromSize_t(result);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_memory_alloc(PyObject *self, PyObject *args)
{
    size_t size;

    if(!PyArg_ParseTuple(args, "n", &size))
    {
        return NULL;
    }

    buff_t result = indi_memory_alloc(size);

    return PyLong_FromVoidPtr(result);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* OBJECT                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_object_parse(PyObject *self, PyObject *args)
{
    STR_t json;

    if(!PyArg_ParseTuple(args, "y", &json))
    {
        return NULL;
    }

    indi_object_t *object = indi_object_parse(json);

    return PyLong_FromVoidPtr(object);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_object_free(PyObject *self, PyObject *args)
{
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "O", &py_object))
    {
        return NULL;
    }

    indi_object_t *object = (indi_object_t *) PyLong_AsVoidPtr(py_object);

    indi_object_free(object);

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_object_to_string(PyObject *self, PyObject *args)
{
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "O", &py_object))
    {
        return NULL;
    }

    indi_object_t *object = (indi_object_t *) PyLong_AsVoidPtr(py_object);

    str_t result = indi_object_to_string(object);
    PyObject *py_result = PyBytes_FromString(result);
    indi_memory_free(result);

    return py_result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* XML                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_xmldoc_parse(PyObject *self, PyObject *args)
{
    STR_t xml;

    if(!PyArg_ParseTuple(args, "y", &xml))
    {
        return NULL;
    }

    indi_xmldoc_t *result = indi_xmldoc_parse(xml);

    return PyLong_FromVoidPtr(result);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_xmldoc_free(PyObject *self, PyObject *args)
{
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "O", &py_object))
    {
        return NULL;
    }

    indi_xmldoc_t *xmldoc = (indi_xmldoc_t *) PyLong_AsVoidPtr(py_object);

    indi_xmldoc_free(xmldoc);

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_xmldoc_to_string(PyObject *self, PyObject *args)
{
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "O", &py_object))
    {
        return NULL;
    }

    indi_xmldoc_t *xmldoc = (indi_xmldoc_t *) PyLong_AsVoidPtr(py_object);

    buff_t result = indi_xmldoc_to_string(xmldoc);
    PyObject *py_result = PyBytes_FromString(result);
    indi_memory_free(result);

    return py_result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* TRANSFORM                                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_xmldoc_to_object(PyObject *self, PyObject *args)
{
    PyObject *py_object;
    int validate;

    if(!PyArg_ParseTuple(args, "Oi", &py_object, &validate))
    {
        return NULL;
    }

    indi_xmldoc_t *xmldoc = (indi_xmldoc_t *) PyLong_AsVoidPtr(py_object);

    indi_object_t *result = indi_xmldoc_to_object(xmldoc, validate);

    return PyLong_FromVoidPtr(result);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_object_to_xmldoc(PyObject *self, PyObject *args)
{
    PyObject *py_object;
    int validate;

    if(!PyArg_ParseTuple(args, "Oi", &py_object, &validate))
    {
        return NULL;
    }

    indi_object_t *object = (indi_object_t *) PyLong_AsVoidPtr(py_object);

    indi_xmldoc_t *result = indi_object_to_xmldoc(object, validate);

    return PyLong_FromVoidPtr(result);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PROXY                                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

static void py_indi_proxy_emit(indi_proxy_t *proxy, size_t size, str_t buff)
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

static PyObject *py_indi_proxy_initialize(PyObject *self, PyObject *args)
{
    size_t size;
    PyObject *py_object;

    if(!PyArg_ParseTuple(args, "nO:", &size, &py_object))
    {
        return NULL;
    }

    if(!PyCallable_Check(py_object))
    {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable");

        return NULL;
    }

    indi_proxy_t *proxy = (indi_proxy_t *) indi_memory_alloc(sizeof(indi_proxy_t));

    indi_proxy_initialize(proxy, size, py_indi_proxy_emit);

    proxy->py = (buff_t) py_object;

    Py_XINCREF(py_object);

    return PyLong_FromVoidPtr(proxy);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_proxy_finalize(PyObject *self, PyObject *args)
{
    PyObject *py_proxy;

    if(!PyArg_ParseTuple(args, "O", &py_proxy))
    {
        return NULL;
    }

    indi_proxy_t *proxy = (indi_proxy_t *) PyLong_AsVoidPtr(py_proxy);

    Py_XDECREF((PyObject *) proxy->py);

    indi_proxy_finalize(proxy);

    indi_memory_free(proxy);

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *py_indi_proxy_consume(PyObject *self, PyObject *args)
{
    PyObject *py_proxy;
    STR_t buff;
    size_t size;

    if(!PyArg_ParseTuple(args, "Oy#", &py_proxy, &buff, &size))
    {
        return NULL;
    }

    indi_proxy_t *proxy = (indi_proxy_t *) PyLong_AsVoidPtr(py_proxy);

    indi_proxy_consume(proxy, (size_t) size, (STR_t) buff);

    Py_RETURN_NONE;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef indi_proxy_methods[] = {
    {"memory_initialize", py_indi_memory_initialize, METH_VARARGS, ""},
    {"memory_finalize", py_indi_memory_finalize, METH_VARARGS, ""},
    {"memory_free", py_indi_memory_free, METH_VARARGS, ""},
    {"memory_alloc", py_indi_memory_alloc, METH_VARARGS, ""},
    /**/
    {"object_parse", py_indi_object_parse, METH_VARARGS, ""},
    {"object_free", py_indi_object_free, METH_VARARGS, ""},
    {"object_to_string", py_indi_object_to_string, METH_VARARGS, ""},
    /**/
    {"xmldoc_parse", py_indi_xmldoc_parse, METH_VARARGS, ""},
    {"xmldoc_free", py_indi_xmldoc_free, METH_VARARGS, ""},
    {"xmldoc_to_string", py_indi_xmldoc_to_string, METH_VARARGS, ""},
    /**/
    {"xmldoc_to_object", py_indi_xmldoc_to_object, METH_VARARGS, ""},
    {"object_to_xmldoc", py_indi_object_to_xmldoc, METH_VARARGS, ""},
    /**/
    {"proxy_initialize", py_indi_proxy_initialize, METH_VARARGS, ""},
    {"proxy_finalize", py_indi_proxy_finalize, METH_VARARGS, ""},
    {"proxy_consume", py_indi_proxy_consume, METH_VARARGS, ""},
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
    return PyModule_Create(&indi_proxy_module);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

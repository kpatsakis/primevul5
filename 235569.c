static PyObject *decode_int64(PyObject *self, PyObject *string)
{
    long long x;
    char *bs;
    int i;

    if (!PyBytes_Check(string)) {
        PyErr_SetString(PyExc_TypeError, "bytes expected");
        return NULL;
    }
    if (PyBytes_Size(string) != 8) {
        PyErr_SetString(PyExc_ValueError, "8 bytes expected");
        return NULL;
    }
    bs = PyBytes_AsString(string);

    x = bs[7] & 0x7F;
    for (i = 6; i >= 0; i--) {
        x <<= 8;  /* x = x * 256 + (unsigned char) bs[i]; */
        x |= (unsigned char) bs[i];
    }
    if (bs[7] & 0x80)
        x = -x;
    return PyLong_FromLongLong(x);
}
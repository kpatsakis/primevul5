static PyObject *encode_int64(PyObject *self, PyObject *value)
{
    long long x;
    char bs[8], sign = 0x00;
    int i;

    if (!PyArg_Parse(value, "L", &x))
        return NULL;

    if (x < 0) {
        x = -x;
        sign = 0x80;
    }
    for (i = 0; i < 8; i++) {
        bs[i] = x & 0xff;
        x >>= 8;  /* x /= 256 */
    }
    bs[7] |= sign;
    return PyBytes_FromStringAndSize(bs, 8);
}
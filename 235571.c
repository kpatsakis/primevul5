static PyObject* diff(PyObject* self, PyObject* args)
{
    off_t lastscan, lastpos, lastoffset, oldscore, scsc, overlap, Ss, lens;
    off_t *I, *V, dblen, eblen, scan, pos, len, s, Sf, lenf, Sb, lenb, i;
    PyObject *controlTuples, *tuple, *results, *temp;
    Py_ssize_t origDataLength, newDataLength;
    char *origData, *newData;
    unsigned char *db, *eb;

    if (!PyArg_ParseTuple(args, "s#s#",
                          &origData, &origDataLength,
                          &newData, &newDataLength))
        return NULL;

    /* create the control tuple */
    controlTuples = PyList_New(0);
    if (!controlTuples)
        return NULL;

    /* perform sort on original data */
    I = PyMem_Malloc((origDataLength + 1) * sizeof(off_t));
    if (!I) {
        Py_DECREF(controlTuples);
        return PyErr_NoMemory();
    }
    V = PyMem_Malloc((origDataLength + 1) * sizeof(off_t));
    if (!V) {
        Py_DECREF(controlTuples);
        PyMem_Free(I);
        return PyErr_NoMemory();
    }
    Py_BEGIN_ALLOW_THREADS  /* release GIL */
    qsufsort(I, V, (unsigned char *) origData, origDataLength);
    Py_END_ALLOW_THREADS
    PyMem_Free(V);

    /* allocate memory for the diff and extra blocks */
    db = PyMem_Malloc(newDataLength + 1);
    if (!db) {
        Py_DECREF(controlTuples);
        PyMem_Free(I);
        return PyErr_NoMemory();
    }
    eb = PyMem_Malloc(newDataLength + 1);
    if (!eb) {
        Py_DECREF(controlTuples);
        PyMem_Free(I);
        PyMem_Free(db);
        return PyErr_NoMemory();
    }
    dblen = 0;
    eblen = 0;

    /* perform the diff */
    len = 0;
    scan = 0;
    lastscan = 0;
    lastpos = 0;
    lastoffset = 0;
    pos = 0;
    while (scan < newDataLength) {
        oldscore = 0;

        Py_BEGIN_ALLOW_THREADS  /* release GIL */
        for (scsc = scan += len; scan < newDataLength; scan++) {
            len = search(I, (unsigned char *) origData, origDataLength,
                         (unsigned char *) newData + scan,
                         newDataLength - scan, 0, origDataLength, &pos);
            for (; scsc < scan + len; scsc++)
                if ((scsc + lastoffset < origDataLength) &&
                          (origData[scsc + lastoffset] == newData[scsc]))
                    oldscore++;
            if (((len == oldscore) && (len != 0)) || (len > oldscore + 8))
                break;
            if ((scan + lastoffset < origDataLength) &&
                      (origData[scan + lastoffset] == newData[scan]))
                oldscore--;
        }
        Py_END_ALLOW_THREADS

        if ((len != oldscore) || (scan == newDataLength)) {
            s = 0;
            Sf = 0;
            lenf = 0;
            for (i = 0; (lastscan + i < scan) &&
                     (lastpos + i < origDataLength);) {
                if (origData[lastpos + i] == newData[lastscan + i])
                    s++;
                i++;
                if (s * 2 - i > Sf * 2 - lenf) {
                    Sf = s;
                    lenf = i;
                }
            }

            lenb = 0;
            if (scan < newDataLength) {
                s = 0;
                Sb = 0;
                for (i = 1; (scan >= lastscan + i) && (pos >= i); i++) {
                    if (origData[pos - i] == newData[scan - i])
                        s++;
                    if (s * 2 - i > Sb * 2 - lenb) {
                        Sb = s;
                        lenb = i;
                    }
                }
            }

            if (lastscan + lenf > scan - lenb) {
                overlap = (lastscan + lenf) - (scan - lenb);
                s = 0;
                Ss = 0;
                lens = 0;
                for (i = 0; i < overlap; i++) {
                    if (newData[lastscan + lenf - overlap + i] ==
                            origData[lastpos + lenf - overlap + i])
                        s++;
                    if (newData[scan - lenb + i]== origData[pos - lenb + i])
                        s--;
                    if (s > Ss) {
                        Ss = s;
                        lens = i + 1;
                    }
                }

                lenf += lens - overlap;
                lenb -= lens;
            }

            for (i = 0; i < lenf; i++)
                db[dblen + i] = newData[lastscan + i] - origData[lastpos + i];
            for (i = 0; i < (scan - lenb) - (lastscan + lenf); i++)
                eb[eblen + i] = newData[lastscan + lenf + i];

            dblen += lenf;
            eblen += (scan - lenb) - (lastscan + lenf);

            tuple = PyTuple_New(3);
            if (!tuple) {
                Py_DECREF(controlTuples);
                PyMem_Free(I);
                PyMem_Free(db);
                PyMem_Free(eb);
                return NULL;
            }
            PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(lenf));
            PyTuple_SET_ITEM(tuple, 1,
                    PyLong_FromLong((scan - lenb) - (lastscan + lenf)));
            PyTuple_SET_ITEM(tuple, 2,
                    PyLong_FromLong((pos - lenb) - (lastpos + lenf)));
            if (PyList_Append(controlTuples, tuple) < 0) {
                Py_DECREF(controlTuples);
                Py_DECREF(tuple);
                PyMem_Free(I);
                PyMem_Free(db);
                PyMem_Free(eb);
                return NULL;
            }
            Py_DECREF(tuple);

            lastscan = scan - lenb;
            lastpos = pos - lenb;
            lastoffset = pos - scan;
        }
    }

    PyMem_Free(I);
    results = PyTuple_New(3);
    if (!results) {
        PyMem_Free(db);
        PyMem_Free(eb);
        return NULL;
    }
    PyTuple_SET_ITEM(results, 0, controlTuples);
    temp = PyBytes_FromStringAndSize((char *) db, dblen);
    PyMem_Free(db);
    if (!temp) {
        PyMem_Free(eb);
        Py_DECREF(results);
        return NULL;
    }
    PyTuple_SET_ITEM(results, 1, temp);
    temp = PyBytes_FromStringAndSize((char *) eb, eblen);
    PyMem_Free(eb);
    if (!temp) {
        Py_DECREF(results);
        return NULL;
    }
    PyTuple_SET_ITEM(results, 2, temp);

    return results;
}
writeCustomTiffTags(TIFF    *tif,
                    NUMA    *natags,
                    SARRAY  *savals,
                    SARRAY  *satypes,
                    NUMA    *nasizes)
{
char      *sval, *type;
l_int32    i, n, ns, size, tagval, val;
l_float64  dval;
l_uint32   uval, uval2;

    PROCNAME("writeCustomTiffTags");

    if (!tif)
        return ERROR_INT("tif stream not defined", procName, 1);
    if (!natags && !savals && !satypes)
        return 0;
    if (!natags || !savals || !satypes)
        return ERROR_INT("not all arrays defined", procName, 1);
    n = numaGetCount(natags);
    if ((sarrayGetCount(savals) != n) || (sarrayGetCount(satypes) != n))
        return ERROR_INT("not all sa the same size", procName, 1);

        /* The sized arrays (4 args to TIFFSetField) are written first */
    if (nasizes) {
        ns = numaGetCount(nasizes);
        if (ns > n)
            return ERROR_INT("too many 4-arg tag calls", procName, 1);
        for (i = 0; i < ns; i++) {
            numaGetIValue(natags, i, &tagval);
            sval = sarrayGetString(savals, i, L_NOCOPY);
            type = sarrayGetString(satypes, i, L_NOCOPY);
            numaGetIValue(nasizes, i, &size);
            if (strcmp(type, "char*") && strcmp(type, "l_uint8*"))
                L_WARNING("array type not char* or l_uint8*; ignore\n",
                          procName);
            TIFFSetField(tif, tagval, size, sval);
        }
    } else {
        ns = 0;
    }

        /* The typical tags (3 args to TIFFSetField) are now written */
    for (i = ns; i < n; i++) {
        numaGetIValue(natags, i, &tagval);
        sval = sarrayGetString(savals, i, L_NOCOPY);
        type = sarrayGetString(satypes, i, L_NOCOPY);
        if (!strcmp(type, "char*") || !strcmp(type, "const char*")) {
            TIFFSetField(tif, tagval, sval);
        } else if (!strcmp(type, "l_uint16")) {
            if (sscanf(sval, "%u", &uval) == 1) {
                TIFFSetField(tif, tagval, (l_uint16)uval);
            } else {
                lept_stderr("val %s not of type %s\n", sval, type);
                return ERROR_INT("custom tag(s) not written", procName, 1);
            }
        } else if (!strcmp(type, "l_uint32")) {
            if (sscanf(sval, "%u", &uval) == 1) {
                TIFFSetField(tif, tagval, uval);
            } else {
                lept_stderr("val %s not of type %s\n", sval, type);
                return ERROR_INT("custom tag(s) not written", procName, 1);
            }
        } else if (!strcmp(type, "l_int32")) {
            if (sscanf(sval, "%d", &val) == 1) {
                TIFFSetField(tif, tagval, val);
            } else {
                lept_stderr("val %s not of type %s\n", sval, type);
                return ERROR_INT("custom tag(s) not written", procName, 1);
            }
        } else if (!strcmp(type, "l_float64")) {
            if (sscanf(sval, "%lf", &dval) == 1) {
                TIFFSetField(tif, tagval, dval);
            } else {
                lept_stderr("val %s not of type %s\n", sval, type);
                return ERROR_INT("custom tag(s) not written", procName, 1);
            }
        } else if (!strcmp(type, "l_uint16-l_uint16")) {
            if (sscanf(sval, "%u-%u", &uval, &uval2) == 2) {
                TIFFSetField(tif, tagval, (l_uint16)uval, (l_uint16)uval2);
            } else {
                lept_stderr("val %s not of type %s\n", sval, type);
                return ERROR_INT("custom tag(s) not written", procName, 1);
            }
        } else {
            lept_stderr("unknown type %s\n",type);
            return ERROR_INT("unknown type; tag(s) not written", procName, 1);
        }
    }
    return 0;
}
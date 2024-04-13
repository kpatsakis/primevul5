static int annotate_canon_value(struct buf *value, int type)
{
    char *p = NULL;
    unsigned long uwhatever = 0;
    long whatever = 0;

    /* check for NIL */
    if (value->s == NULL)
        return 0;

    switch (type) {
    case ATTRIB_TYPE_STRING:
        /* free form */
        break;

    case ATTRIB_TYPE_BOOLEAN:
        /* make sure its "true" or "false" */
        if (value->len == 4 && !strncasecmp(value->s, "true", 4)) {
            buf_reset(value);
            buf_appendcstr(value, "true");
            buf_cstring(value);
        }
        else if (value->len == 5 && !strncasecmp(value->s, "false", 5)) {
            buf_reset(value);
            buf_appendcstr(value, "false");
            buf_cstring(value);
        }
        else return IMAP_ANNOTATION_BADVALUE;
        break;

    case ATTRIB_TYPE_UINT:
        /* make sure its a valid ulong ( >= 0 ) */
        errno = 0;
        buf_cstring(value);
        uwhatever = strtoul(value->s, &p, 10);
        if ((p == value->s)             /* no value */
            || (*p != '\0')             /* illegal char */
            || (unsigned)(p - value->s) != value->len
                                        /* embedded NUL */
            || errno                    /* overflow */
            || strchr(value->s, '-')) { /* negative number */
            return IMAP_ANNOTATION_BADVALUE;
        }
        break;

    case ATTRIB_TYPE_INT:
        /* make sure its a valid long */
        errno = 0;
        buf_cstring(value);
        whatever = strtol(value->s, &p, 10);
        if ((p == value->s)             /* no value */
            || (*p != '\0')             /* illegal char */
            || (unsigned)(p - value->s) != value->len
                                        /* embedded NUL */
            || errno) {                 /* underflow/overflow */
            return IMAP_ANNOTATION_BADVALUE;
        }
        break;

    default:
        /* unknown type */
        return IMAP_ANNOTATION_BADVALUE;
    }

    if (whatever || uwhatever) /* filthy compiler magic */
        return 0;

    return 0;
}
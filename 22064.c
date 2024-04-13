status_put_id(stream * s, const char *title, const char *id)
{                               /* HACK: we know that there's at least one character in the buffer. */
    if (*s->cursor.w.ptr == '\n') {     /* We haven't started the list yet. */
        stprintf(s, "%s=\"%s", title, id);
    } else {
        stprintf(s, ",%s", id);
    }
}
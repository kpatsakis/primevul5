status_end_id_list(stream * s)
{                               /* HACK: we know that there's at least one character in the buffer. */
    if (*s->cursor.w.ptr != '\n')
        stputs(s, "\"\r\n");
}
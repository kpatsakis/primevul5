EXPORTED void annotatemore_open(void)
{
    int r;
    annotate_db_t *d = NULL;

    /* force opening the global annotations db */
    r = _annotate_getdb(NULL, 0, CYRUSDB_CREATE, &d);
    if (r)
        fatal("can't open global annotations database", EX_TEMPFAIL);

    annotatemore_dbopen = 1;
}
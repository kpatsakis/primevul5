EXPORTED void annotate_done(void)
{
    /* DB->done() handled by cyrus_done() */
    if (annotatemore_dbopen) {
        annotatemore_close();
    }
    annotate_initialized = 0;
}
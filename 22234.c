static void done_cb(void*rock __attribute__((unused)))
{
    if (annotatemore_dbopen) {
        annotatemore_close();
    }
    annotate_done();
}
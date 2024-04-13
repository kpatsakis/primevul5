static void init_internal()
{
    if (!annotate_initialized) {
        annotate_init(NULL, NULL);
        cyrus_modules_add(done_cb, NULL);
    }
    if (!annotatemore_dbopen) {
        annotatemore_open();
    }
}
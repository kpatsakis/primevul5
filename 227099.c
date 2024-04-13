static void jbig2_global_data_finalize(const gs_memory_t *cmem, void *vptr)
{
        s_jbig2_global_data_t *st = vptr;
        (void)cmem; /* unused */

        if (st->data) s_jbig2decode_free_global_data(st->data);
        st->data = NULL;
}
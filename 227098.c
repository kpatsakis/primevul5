z_jbig2makeglobalctx(i_ctx_t * i_ctx_p)
{
        void *global = NULL;
        s_jbig2_global_data_t *st;
        os_ptr op = osp;
        byte *data;
        int size;
        int code = 0;

        check_type(*op, t_astruct);
        size = gs_object_size(imemory, op->value.pstruct);
        data = r_ptr(op, byte);

        code = s_jbig2decode_make_global_data(data, size,
                        &global);
        if (size > 0 && global == NULL) {
            dmlprintf(imemory, "failed to create parsed JBIG2GLOBALS object.");
            return_error(gs_error_unknownerror);
        }

        st = ialloc_struct(s_jbig2_global_data_t,
                &st_jbig2_global_data_t,
                "jbig2decode parsed global context");
        if (st == NULL) return_error(gs_error_VMerror);

        st->data = global;
        make_astruct(op, a_readonly | icurrent_space, (byte*)st);

        return code;
}
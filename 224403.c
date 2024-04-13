gs_interpret(i_ctx_t **pi_ctx_p, ref * pref, int user_errors, int *pexit_code,
             ref * perror_object)
{
    i_ctx_t *i_ctx_p = *pi_ctx_p;
    gs_gc_root_t error_root;
    int code;

    gs_register_ref_root(imemory_system, &error_root,
                         (void **)&perror_object, "gs_interpret");
    code = gs_call_interp(pi_ctx_p, pref, user_errors, pexit_code,
                          perror_object);
    i_ctx_p = *pi_ctx_p;
    gs_unregister_root(imemory_system, &error_root, "gs_interpret");
    /* Avoid a dangling reference to the lib context GC signal. */
    set_gc_signal(i_ctx_p, 0);
    return code;
}
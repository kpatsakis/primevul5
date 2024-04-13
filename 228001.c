zoutputpage(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;

    check_type(op[-1], t_integer);
    check_type(*op, t_boolean);
    if (gs_debug[':']) {
        gs_main_instance *minst = get_minst_from_memory((gs_memory_t *)i_ctx_p->memory.current->non_gc_memory);

        print_resource_usage(minst, &(i_ctx_p->memory), "Outputpage start");
    }
    code = gs_output_page(igs, (int)op[-1].value.intval,
                          op->value.boolval);
    if (code < 0)
        return code;
    pop(2);
    if (gs_debug[':']) {
        gs_main_instance *minst = get_minst_from_memory((gs_memory_t *)i_ctx_p->memory.current->non_gc_memory);

        print_resource_usage(minst, &(i_ctx_p->memory), "Outputpage end");
    }
    return 0;
}
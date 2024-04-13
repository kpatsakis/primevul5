static void invalidate_stack_devices(i_ctx_t *i_ctx_p)
{
    os_ptr op = osbot;
    while (op != ostop) {
        if (r_has_type(op, t_device))
            op->value.pdevice = 0;
        op++;
    }
}

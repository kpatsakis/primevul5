zputdeviceparams(i_ctx_t *i_ctx_p)
{
    uint count = ref_stack_counttomark(&o_stack);
    ref *prequire_all;
    ref *ppolicy;
    ref *pdev;
    gx_device *dev;
    stack_param_list list;
    int code;
    int old_width, old_height;
    int i, dest;

    if (count == 0)
        return_error(gs_error_unmatchedmark);
    prequire_all = ref_stack_index(&o_stack, count);
    ppolicy = ref_stack_index(&o_stack, count + 1);
    pdev = ref_stack_index(&o_stack, count + 2);
    if (pdev == 0)
        return_error(gs_error_stackunderflow);
    check_type_only(*prequire_all, t_boolean);
    check_write_type_only(*pdev, t_device);
    dev = pdev->value.pdevice;
    if (dev == NULL)
        /* This can happen if we invalidated devices on the stack by calling nulldevice after they were pushed */
        return_error(gs_error_undefined);
    code = stack_param_list_read(&list, &o_stack, 0, ppolicy,
                                 prequire_all->value.boolval, iimemory);
    if (code < 0)
        return code;
    old_width = dev->width;
    old_height = dev->height;
    code = gs_putdeviceparams(dev, (gs_param_list *) & list);
    /* Check for names that were undefined or caused errors. */
    for (dest = count - 2, i = 0; i < count >> 1; i++)
        if (list.results[i] < 0) {
            *ref_stack_index(&o_stack, dest) =
                *ref_stack_index(&o_stack, count - (i << 1) - 2);
            gs_errorname(i_ctx_p, list.results[i],
                         ref_stack_index(&o_stack, dest - 1));
            dest -= 2;
        }
    iparam_list_release(&list);
    if (code < 0) {		/* There were errors reported. */
        ref_stack_pop(&o_stack, dest + 1);
        return (code == gs_error_Fatal) ? code : 0;	/* cannot continue from Fatal */
    }
    if (code > 0 || (code == 0 && (dev->width != old_width || dev->height != old_height))) {
        /*
         * The device was open and is now closed, or its dimensions have
         * changed.  If it was the current device, call setdevice to
         * reinstall it and erase the page.
         */
        /****** DOESN'T FIND ALL THE GSTATES THAT REFERENCE THE DEVICE. ******/
        if (gs_currentdevice(igs) == dev) {
            bool was_open = dev->is_open;

            code = gs_setdevice_no_erase(igs, dev);
            /* If the device wasn't closed, setdevice won't erase the page. */
            if (was_open && code >= 0)
                code = 1;
        }
    }
    if (code < 0)
        return code;
    ref_stack_pop(&o_stack, count + 1);
    make_bool(osp, code);
    clear_pagedevice(istate);
    return 0;
}
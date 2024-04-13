pcl_echo(pcl_args_t * pargs, pcl_state_t * pcs)
{
    stream st;

    status_begin(&st, pcs);
    stprintf(&st, "ECHO %d\r\n", int_arg(pargs));
    status_end(&st, pcs);
    return 0;
}
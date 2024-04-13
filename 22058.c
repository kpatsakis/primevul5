pcl_free_space(pcl_args_t * pargs, pcl_state_t * pcs)
{
    stream st;

    status_begin(&st, pcs);
    stprintf(&st, "INFO MEMORY\r\n");
    if (int_arg(pargs) != 1)
        stprintf(&st, "ERROR=INVALID UNIT\r\n");
    else {
        gs_memory_status_t mstat;

        gs_memory_status(pcs->memory, &mstat);
        if (pcs->memory != pcs->memory->non_gc_memory) {
            gs_memory_status_t dstat;

            gs_memory_status(pcs->memory->non_gc_memory, &dstat);
            mstat.allocated += dstat.allocated;
            mstat.used += dstat.used;
        }
        stprintf(&st, "TOTAL=%ld\r\n", mstat.allocated - mstat.used);
        /* We don't currently have an API for determining */
        /* the largest contiguous block. */
            /**** RETURN SOMETHING RANDOM ****/
        stprintf(&st, "LARGEST=%ld\r\n", (mstat.allocated - mstat.used) >> 2);
    }
    status_end(&st, pcs);
    return 0;
}
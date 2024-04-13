static void reds_exit(void)
{
    if (reds->main_channel) {
        main_channel_close(reds->main_channel);
    }
#ifdef RED_STATISTICS
    shm_unlink(reds->stat_shm_name);
    free(reds->stat_shm_name);
#endif
}

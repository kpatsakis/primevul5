static void tcg_log_global_after_sync(MemoryListener *listener)
{
    CPUAddressSpace *cpuas;

    /* Wait for the CPU to end the current TB.  This avoids the following
     * incorrect race:
     *
     *      vCPU                         migration
     *      ----------------------       -------------------------
     *      TLB check -> slow path
     *        notdirty_mem_write
     *          write to RAM
     *          mark dirty
     *                                   clear dirty flag
     *      TLB check -> fast path
     *                                   read memory
     *        write to RAM
     *
     * by pushing the migration thread's memory read after the vCPU thread has
     * written the memory.
     */
    if (replay_mode == REPLAY_MODE_NONE) {
        /*
         * VGA can make calls to this function while updating the screen.
         * In record/replay mode this causes a deadlock, because
         * run_on_cpu waits for rr mutex. Therefore no races are possible
         * in this case and no need for making run_on_cpu when
         * record/replay is enabled.
         */
        cpuas = container_of(listener, CPUAddressSpace, tcg_as_listener);
        run_on_cpu(cpuas->cpu, do_nothing, RUN_ON_CPU_NULL);
    }
}
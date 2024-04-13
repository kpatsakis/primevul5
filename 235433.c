static void ram_block_discard_disable_mutex_lock(void)
{
    static gsize initialized;

    if (g_once_init_enter(&initialized)) {
        qemu_mutex_init(&ram_block_discard_disable_mutex);
        g_once_init_leave(&initialized, 1);
    }
    qemu_mutex_lock(&ram_block_discard_disable_mutex);
}
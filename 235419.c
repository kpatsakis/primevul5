static void ram_block_discard_disable_mutex_unlock(void)
{
    qemu_mutex_unlock(&ram_block_discard_disable_mutex);
}
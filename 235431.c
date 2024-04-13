int ram_block_discard_disable(bool state)
{
    int ret = 0;

    ram_block_discard_disable_mutex_lock();
    if (!state) {
        ram_block_discard_disabled_cnt--;
    } else if (ram_block_discard_required_cnt ||
               ram_block_coordinated_discard_required_cnt) {
        ret = -EBUSY;
    } else {
        ram_block_discard_disabled_cnt++;
    }
    ram_block_discard_disable_mutex_unlock();
    return ret;
}
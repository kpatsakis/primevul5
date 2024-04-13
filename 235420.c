bool ram_block_discard_is_required(void)
{
    return qatomic_read(&ram_block_discard_required_cnt) ||
           qatomic_read(&ram_block_coordinated_discard_required_cnt);
}
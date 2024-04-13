bool ram_block_discard_is_disabled(void)
{
    return qatomic_read(&ram_block_discard_disabled_cnt) ||
           qatomic_read(&ram_block_uncoordinated_discard_disabled_cnt);
}
onig_memsize(const regex_t *reg)
{
    size_t size = sizeof(regex_t);
    if (IS_NOT_NULL(reg->p))                size += reg->alloc;
    if (IS_NOT_NULL(reg->exact))            size += reg->exact_end - reg->exact;
    if (IS_NOT_NULL(reg->int_map))          size += sizeof(int) * ONIG_CHAR_TABLE_SIZE;
    if (IS_NOT_NULL(reg->int_map_backward)) size += sizeof(int) * ONIG_CHAR_TABLE_SIZE;
    if (IS_NOT_NULL(reg->repeat_range))     size += reg->repeat_range_alloc * sizeof(OnigRepeatRange);
    if (IS_NOT_NULL(reg->chain))            size += onig_memsize(reg->chain);

    return size;
}
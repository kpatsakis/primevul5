GString *ram_block_format(void)
{
    RAMBlock *block;
    char *psize;
    GString *buf = g_string_new("");

    RCU_READ_LOCK_GUARD();
    g_string_append_printf(buf, "%24s %8s  %18s %18s %18s\n",
                           "Block Name", "PSize", "Offset", "Used", "Total");
    RAMBLOCK_FOREACH(block) {
        psize = size_to_str(block->page_size);
        g_string_append_printf(buf, "%24s %8s  0x%016" PRIx64 " 0x%016" PRIx64
                               " 0x%016" PRIx64 "\n", block->idstr, psize,
                               (uint64_t)block->offset,
                               (uint64_t)block->used_length,
                               (uint64_t)block->max_length);
        g_free(psize);
    }

    return buf;
}
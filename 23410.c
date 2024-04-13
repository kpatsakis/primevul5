static inline void print_gem_tx_desc(uint32_t *desc, uint8_t queue)
{
    DB_PRINT("TXDESC (queue %" PRId8 "):\n", queue);
    DB_PRINT("bufaddr: 0x%08x\n", *desc);
    DB_PRINT("used_hw: %d\n", tx_desc_get_used(desc));
    DB_PRINT("wrap:    %d\n", tx_desc_get_wrap(desc));
    DB_PRINT("last:    %d\n", tx_desc_get_last(desc));
    DB_PRINT("length:  %d\n", tx_desc_get_length(desc));
}
static uint32_t get_local_mem_size_index(uint32_t size)
{
    uint32_t norm_size = 0;
    int i, index = 0;

    for (i = 0; i < ARRAY_SIZE(sm501_mem_local_size); i++) {
        uint32_t new_size = sm501_mem_local_size[i];
        if (new_size >= size) {
            if (norm_size == 0 || norm_size > new_size) {
                norm_size = new_size;
                index = i;
            }
        }
    }

    return index;
}
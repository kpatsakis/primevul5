st_hash_uint32(st_index_t h, uint32_t i)
{
    return murmur_step(h + i, 16);
}
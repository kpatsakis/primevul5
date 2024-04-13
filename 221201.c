rb_memhash(const void *ptr, long len)
{
    return st_hash(ptr, len, rb_hash_start(0));
}
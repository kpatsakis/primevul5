static void pcre2_jit_free(void *ptr, void *allocator_data)
{
pcre2_memctl *allocator = ((pcre2_memctl*)allocator_data);
allocator->free(ptr, allocator->memory_data);
}
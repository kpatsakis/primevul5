static void * pcre2_jit_malloc(size_t size, void *allocator_data)
{
pcre2_memctl *allocator = ((pcre2_memctl*)allocator_data);
return allocator->malloc(size, allocator->memory_data);
}
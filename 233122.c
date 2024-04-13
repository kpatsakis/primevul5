void *zmalloc_usable(size_t size, size_t *usable) {
    void *ptr = malloc(size+PREFIX_SIZE);

    if (!ptr) zmalloc_oom_handler(size);
#ifdef HAVE_MALLOC_SIZE
    update_zmalloc_stat_alloc(*usable = zmalloc_size(ptr));
    return ptr;
#else
    *((size_t*)ptr) = *usable = size;
    update_zmalloc_stat_alloc(size+PREFIX_SIZE);
    return (char*)ptr+PREFIX_SIZE;
#endif
}
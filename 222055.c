static AsyncURB *async_alloc(void)
{
    return (AsyncURB *) qemu_mallocz(sizeof(AsyncURB));
}
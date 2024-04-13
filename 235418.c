bool qemu_ram_is_noreserve(RAMBlock *rb)
{
    return rb->flags & RAM_NORESERVE;
}
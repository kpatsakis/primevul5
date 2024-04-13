long qemu_maxrampagesize(void)
{
    return qemu_real_host_page_size();
}
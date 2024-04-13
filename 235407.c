long qemu_minrampagesize(void)
{
    return qemu_real_host_page_size();
}
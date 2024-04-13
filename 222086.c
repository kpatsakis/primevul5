static USBHostDevice *hostdev_find(int bus_num, int addr)
{
    USBHostDevice *s = hostdev_list;
    while (s) {
        if (s->bus_num == bus_num && s->addr == addr)
            return s;
        s = s->next;
    }
    return NULL;
}
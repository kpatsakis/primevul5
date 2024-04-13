static void hostdev_link(USBHostDevice *dev)
{
    dev->next = hostdev_list;
    hostdev_list = dev;
}
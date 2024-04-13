static void async_cancel(USBPacket *unused, void *opaque)
{
    AsyncURB *aurb = opaque;
    USBHostDevice *s = aurb->hdev;

    dprintf("husb: async cancel. aurb %p\n", aurb);

    /* Mark it as dead (see async_complete above) */
    aurb->packet = NULL;

    int r = ioctl(s->fd, USBDEVFS_DISCARDURB, aurb);
    if (r < 0) {
        dprintf("husb: async. discard urb failed errno %d\n", errno);
    }
}
static int is_isoc(USBHostDevice *s, int ep)
{
    return s->endp_table[ep - 1].type == USBDEVFS_URB_TYPE_ISO;
}
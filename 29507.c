static void reds_handle_link_error(void *opaque, int err)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    switch (err) {
    case 0:
    case EPIPE:
        break;
    default:
        spice_warning("%s", strerror(errno));
        break;
    }
    reds_link_free(link);
}

static ssize_t vnc_client_read_plain(VncState *vs)
{
    ssize_t ret;
    VNC_DEBUG("Read plain %p size %zd offset %zd\n",
              vs->input.buffer, vs->input.capacity, vs->input.offset);
    buffer_reserve(&vs->input, 4096);
    ret = vnc_client_read_buf(vs, buffer_end(&vs->input), 4096);
    if (!ret)
        return 0;
    vs->input.offset += ret;
    return ret;
}
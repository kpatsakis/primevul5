status_begin(stream * s, pcl_state_t * pcs)
{
    byte *buffer = pcs->status.buffer;

    if (pcs->status.read_pos > 0) {
        memmove(buffer, buffer + pcs->status.read_pos,
                pcs->status.write_pos - pcs->status.read_pos);
        pcs->status.write_pos -= pcs->status.read_pos;
        pcs->status.read_pos = 0;
    }
    if (buffer == 0) {
        buffer = gs_alloc_bytes(pcs->memory, STATUS_BUFFER_SIZE,
                                "status buffer");
        pcs->status.buffer = buffer;
    }
    if (buffer == 0)
        swrite_string(s, pcs->status.internal_buffer,
                      sizeof(pcs->status.internal_buffer));
    else
        swrite_string(s, buffer, gs_object_size(pcs->memory, buffer));
    sseek(s, pcs->status.write_pos);
    stputs(s, "PCL\r\n");
}
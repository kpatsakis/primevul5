static int read_new_config_info (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    int bytecnt = wpmd->byte_length;
    unsigned char *byteptr = wpmd->data;

    wpc->version_five = 1;      // just having this block signals version 5.0

    wpc->file_format = wpc->config.qmode = wpc->channel_layout = 0;

    if (wpc->channel_reordering) {
        free (wpc->channel_reordering);
        wpc->channel_reordering = NULL;
    }

    // if there's any data, the first two bytes are file_format and qmode flags

    if (bytecnt) {
        wpc->file_format = *byteptr++;
        wpc->config.qmode = (wpc->config.qmode & ~0xff) | *byteptr++;
        bytecnt -= 2;

        // another byte indicates a channel layout

        if (bytecnt) {
            int nchans, i;

            wpc->channel_layout = (int32_t) *byteptr++ << 16;
            bytecnt--;

            // another byte means we have a channel count for the layout and maybe a reordering

            if (bytecnt) {
                wpc->channel_layout += nchans = *byteptr++;
                bytecnt--;

                // any more means there's a reordering string

                if (bytecnt) {
                    if (bytecnt > nchans)
                        return FALSE;

                    wpc->channel_reordering = malloc (nchans);

                    // note that redundant reordering info is not stored, so we fill in the rest

                    if (wpc->channel_reordering) {
                        for (i = 0; i < nchans; ++i)
                            if (bytecnt) {
                                wpc->channel_reordering [i] = *byteptr++;
                                bytecnt--;
                            }
                            else
                                wpc->channel_reordering [i] = i;
                    }
                }
            }
            else
                wpc->channel_layout += wpc->config.num_channels;
        }
    }

    return TRUE;
}
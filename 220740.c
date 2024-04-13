int vnc_send_framebuffer_update(VncState *vs, int x, int y, int w, int h)
{
    int n = 0;
    bool encode_raw = false;
    size_t saved_offs = vs->output.offset;

    switch(vs->vnc_encoding) {
        case VNC_ENCODING_ZLIB:
            n = vnc_zlib_send_framebuffer_update(vs, x, y, w, h);
            break;
        case VNC_ENCODING_HEXTILE:
            vnc_framebuffer_update(vs, x, y, w, h, VNC_ENCODING_HEXTILE);
            n = vnc_hextile_send_framebuffer_update(vs, x, y, w, h);
            break;
        case VNC_ENCODING_TIGHT:
            n = vnc_tight_send_framebuffer_update(vs, x, y, w, h);
            break;
        case VNC_ENCODING_TIGHT_PNG:
            n = vnc_tight_png_send_framebuffer_update(vs, x, y, w, h);
            break;
        case VNC_ENCODING_ZRLE:
            n = vnc_zrle_send_framebuffer_update(vs, x, y, w, h);
            break;
        case VNC_ENCODING_ZYWRLE:
            n = vnc_zywrle_send_framebuffer_update(vs, x, y, w, h);
            break;
        default:
            encode_raw = true;
            break;
    }

    /* If the client has the same pixel format as our internal buffer and
     * a RAW encoding would need less space fall back to RAW encoding to
     * save bandwidth and processing power in the client. */
    if (!encode_raw && vs->write_pixels == vnc_write_pixels_copy &&
        12 + h * w * VNC_SERVER_FB_BYTES <= (vs->output.offset - saved_offs)) {
        vs->output.offset = saved_offs;
        encode_raw = true;
    }

    if (encode_raw) {
        vnc_framebuffer_update(vs, x, y, w, h, VNC_ENCODING_RAW);
        n = vnc_raw_send_framebuffer_update(vs, x, y, w, h);
    }

    return n;
}
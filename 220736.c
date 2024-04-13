int vnc_display_pw_expire(const char *id, time_t expires)
{
    VncDisplay *vs = vnc_display_find(id);

    if (!vs) {
        return -EINVAL;
    }

    vs->expires = expires;
    return 0;
}
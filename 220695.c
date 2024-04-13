static VncDisplay *vnc_display_find(const char *id)
{
    VncDisplay *vd;

    if (id == NULL) {
        return QTAILQ_FIRST(&vnc_displays);
    }
    QTAILQ_FOREACH(vd, &vnc_displays, next) {
        if (strcmp(id, vd->id) == 0) {
            return vd;
        }
    }
    return NULL;
}
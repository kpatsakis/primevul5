static int usb_host_read_file(char *line, size_t line_size, const char *device_file, const char *device_name)
{
    Monitor *mon = cur_mon;
    FILE *f;
    int ret = 0;
    char filename[PATH_MAX];

    snprintf(filename, PATH_MAX, USBSYSBUS_PATH "/devices/%s/%s", device_name,
             device_file);
    f = fopen(filename, "r");
    if (f) {
        fgets(line, line_size, f);
        fclose(f);
        ret = 1;
    } else {
        monitor_printf(mon, "husb: could not open %s\n", filename);
    }

    return ret;
}
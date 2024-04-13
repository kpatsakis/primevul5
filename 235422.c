static int64_t get_file_align(int fd)
{
    int64_t align = -1;
#if defined(__linux__) && defined(CONFIG_LIBDAXCTL)
    struct stat st;

    if (fstat(fd, &st) < 0) {
        return -errno;
    }

    /* Special handling for devdax character devices */
    if (S_ISCHR(st.st_mode)) {
        g_autofree char *path = NULL;
        g_autofree char *rpath = NULL;
        struct daxctl_ctx *ctx;
        struct daxctl_region *region;
        int rc = 0;

        path = g_strdup_printf("/sys/dev/char/%d:%d",
                    major(st.st_rdev), minor(st.st_rdev));
        rpath = realpath(path, NULL);
        if (!rpath) {
            return -errno;
        }

        rc = daxctl_new(&ctx);
        if (rc) {
            return -1;
        }

        daxctl_region_foreach(ctx, region) {
            if (strstr(rpath, daxctl_region_get_path(region))) {
                align = daxctl_region_get_align(region);
                break;
            }
        }
        daxctl_unref(ctx);
    }
#endif /* defined(__linux__) && defined(CONFIG_LIBDAXCTL) */

    return align;
}
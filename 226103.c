static int xar_cleanup_temp_file(cli_ctx *ctx, int fd, char * tmpname)
{
    int rc = CL_SUCCESS;
    if (fd > -1)
        close(fd);
    if (tmpname != NULL) {
        if (!ctx->engine->keeptmp) {
            if (cli_unlink(tmpname)) {
                cli_dbgmsg("cli_scanxar: error unlinking tmpfile %s\n", tmpname); 
                rc = CL_EUNLINK;
            }
        }
        free(tmpname);
    }
    return rc;
}
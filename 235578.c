int src_parser_cpp(const char *src, const struct trans_config cfg)
{
    int tmp_fd;
    char fname[TMP_FILE_NAME_SIZE];

    strncpy(fname, TMP_FILE_NAME, TMP_FILE_NAME_SIZE);
    tmp_fd = mkstemp(fname);
    if (tmp_fd == -1) {
        fprintf(stderr, "**Error: could not create a working file.\n");
        return -1;
    }

    src_parser_trans_stage_1_2_3(tmp_fd, src, cfg);

    print_file_full(tmp_fd);

    unlink(fname);
}
static void print_file_full(int fd)
{
    char f_buf[SRC_PARSER_F_BUF_SIZE];
    int read_size;

    if (lseek(fd, 0, SEEK_SET)) {
        fprintf(stderr, "**Error: Could not set offset.\n");
        return;
    }

    while ((read_size = read(fd, f_buf, SRC_PARSER_F_BUF_SIZE)) > 0) {
        int read_indx = 0;

        while (read_indx < read_size)
            putchar(f_buf[read_indx++]);
    }
}
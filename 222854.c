static int get_next_eof(FILE *fp)
{
    int match, c;
    const char buf[] = "%%EOF";

    match = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == buf[match])
          ++match;
        else
          match = 0;

        if (match == 5) /* strlen("%%EOF") */
          return ftell(fp) - 5;
    }

    return -1;
}
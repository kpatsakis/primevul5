__zzip_aligned4(char *p)
{
#define aligned4   __zzip_aligned4
    p += ((long) p) & 1;        /* warnings about truncation of a "pointer" */
    p += ((long) p) & 2;        /* to a "long int" may be safely ignored :) */
    return p;
}
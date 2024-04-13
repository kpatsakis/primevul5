static unsigned int read_le16(const unsigned char *p)
{
    return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8);
}
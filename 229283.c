mysql_escape_string(char *to,const char *from, ulong length)
{
    return (ulong)mysql_cset_escape_slashes(ma_default_charset_info, to, from, length);
}
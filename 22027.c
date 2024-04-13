PJ_DEF(int) pj_stun_set_padding_char(int chr)
{
    int old_pad = padding_char;
    padding_char = chr;
    return old_pad;
}
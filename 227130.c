uint8_t *bcf_fmt_sized_array(kstring_t *s, uint8_t *ptr)
{
    int x, type;
    x = bcf_dec_size(ptr, &ptr, &type);
    bcf_fmt_array(s, x, type, ptr);
    return ptr + (x << bcf_type_shift[type]);
}
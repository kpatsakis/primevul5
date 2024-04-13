rb_str_hash_m(VALUE str)
{
    st_index_t hval = rb_str_hash(str);
    return INT2FIX(hval);
}
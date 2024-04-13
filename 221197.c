rb_str_eql(VALUE str1, VALUE str2)
{
    if (TYPE(str2) != T_STRING) return Qfalse;
    return str_eql(str1, str2);
}
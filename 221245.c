rb_str_replace(VALUE str, VALUE str2)
{
    if (str == str2) return str;

    StringValue(str2);
    str_discard(str);
    return str_replace(str, str2);
}
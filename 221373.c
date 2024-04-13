str_independent(VALUE str)
{
    str_modifiable(str);
    if (!STR_SHARED_P(str)) return 1;
    if (STR_EMBED_P(str)) return 1;
    return 0;
}
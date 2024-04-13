static void str_swap(pj_str_t *str1, pj_str_t *str2)
{
    pj_str_t tmp = *str1;
    *str1 = *str2;
    *str2 = tmp;
}
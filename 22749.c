Init_gb2312(void)
{
    rb_enc_register("GB2312", rb_enc_find("EUC-KR"));
}
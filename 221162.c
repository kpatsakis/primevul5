rb_str_crypt(VALUE str, VALUE salt)
{
    extern char *crypt(const char *, const char *);
    VALUE result;
    const char *s, *saltp;
#ifdef BROKEN_CRYPT
    char salt_8bit_clean[3];
#endif

    StringValue(salt);
    if (RSTRING_LEN(salt) < 2)
	rb_raise(rb_eArgError, "salt too short (need >=2 bytes)");

    s = RSTRING_PTR(str);
    if (!s) s = "";
    saltp = RSTRING_PTR(salt);
#ifdef BROKEN_CRYPT
    if (!ISASCII((unsigned char)saltp[0]) || !ISASCII((unsigned char)saltp[1])) {
	salt_8bit_clean[0] = saltp[0] & 0x7f;
	salt_8bit_clean[1] = saltp[1] & 0x7f;
	salt_8bit_clean[2] = '\0';
	saltp = salt_8bit_clean;
    }
#endif
    result = rb_str_new2(crypt(s, saltp));
    OBJ_INFECT(result, str);
    OBJ_INFECT(result, salt);
    return result;
}
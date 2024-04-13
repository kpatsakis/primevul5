enc_succ_alnum_char(char *p, long len, rb_encoding *enc, char *carry)
{
    enum neighbor_char ret;
    unsigned int c;
    int ctype;
    int range;
    char save[ONIGENC_CODE_TO_MBC_MAXLEN];

    c = rb_enc_mbc_to_codepoint(p, p+len, enc);
    if (rb_enc_isctype(c, ONIGENC_CTYPE_DIGIT, enc))
        ctype = ONIGENC_CTYPE_DIGIT;
    else if (rb_enc_isctype(c, ONIGENC_CTYPE_ALPHA, enc))
        ctype = ONIGENC_CTYPE_ALPHA;
    else
        return NEIGHBOR_NOT_CHAR;

    MEMCPY(save, p, char, len);
    ret = enc_succ_char(p, len, enc);
    if (ret == NEIGHBOR_FOUND) {
        c = rb_enc_mbc_to_codepoint(p, p+len, enc);
        if (rb_enc_isctype(c, ctype, enc))
            return NEIGHBOR_FOUND;
    }
    MEMCPY(p, save, char, len);
    range = 1;
    while (1) {
        MEMCPY(save, p, char, len);
        ret = enc_pred_char(p, len, enc);
        if (ret == NEIGHBOR_FOUND) {
            c = rb_enc_mbc_to_codepoint(p, p+len, enc);
            if (!rb_enc_isctype(c, ctype, enc)) {
                MEMCPY(p, save, char, len);
                break;
            }
        }
        else {
            MEMCPY(p, save, char, len);
            break;
        }
        range++;
    }
    if (range == 1) {
        return NEIGHBOR_NOT_CHAR;
    }

    if (ctype != ONIGENC_CTYPE_DIGIT) {
        MEMCPY(carry, p, char, len);
        return NEIGHBOR_WRAPPED;
    }

    MEMCPY(carry, p, char, len);
    enc_succ_char(carry, len, enc);
    return NEIGHBOR_WRAPPED;
}
static char *base64_encode(char *ostr, long ch) {

    *ostr++ = base64[(ch>>18)&0x3f];
    *ostr++ = base64[(ch>>12)&0x3f];
    *ostr++ = base64[(ch>> 6)&0x3f];
    *ostr++ = base64[(ch    )&0x3f];
return( ostr );
}
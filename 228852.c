static void SFDGetHintMask(FILE *sfd,HintMask *hintmask) {
    int nibble = 0, ch;

    memset(hintmask,0,sizeof(HintMask));
    for (;;) {
	ch = nlgetc(sfd);
	if ( isdigit(ch))
	    ch -= '0';
	else if ( ch>='a' && ch<='f' )
	    ch -= 'a'-10;
	else if ( ch>='A' && ch<='F' )
	    ch -= 'A'-10;
	else {
	    ungetc(ch,sfd);
    break;
	}
	if ( nibble<2*HntMax/8 )
	    (*hintmask)[nibble>>1] |= ch<<(4*(1-(nibble&1)));
	++nibble;
    }
}
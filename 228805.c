static int Dec85(struct enc85 *dec) {
    int ch1, ch2, ch3, ch4, ch5;
    unsigned int val;

    if ( dec->pos<0 ) {
	while ( isspace(ch1=getc(dec->sfd)));
	if ( ch1=='z' ) {
	    dec->sofar[0] = dec->sofar[1] = dec->sofar[2] = dec->sofar[3] = 0;
	    dec->pos = 3;
	} else {
	    while ( isspace(ch2=getc(dec->sfd)));
	    while ( isspace(ch3=getc(dec->sfd)));
	    while ( isspace(ch4=getc(dec->sfd)));
	    while ( isspace(ch5=getc(dec->sfd)));
	    val = ((((ch1-'!')*85+ ch2-'!')*85 + ch3-'!')*85 + ch4-'!')*85 + ch5-'!';
	    dec->sofar[3] = val>>24;
	    dec->sofar[2] = val>>16;
	    dec->sofar[1] = val>>8;
	    dec->sofar[0] = val;
	    dec->pos = 3;
	}
    }
return( dec->sofar[dec->pos--] );
}
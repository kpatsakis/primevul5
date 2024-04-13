static void SFDEnc85EndEnc(struct enc85 *enc) {
    int i;
    int ch2, ch3, ch4, ch5;
    unsigned val;
    if ( enc->pos==0 )
return;
    for ( i=enc->pos; i<4; ++i )
	enc->sofar[i] = 0;
    val = (enc->sofar[0]<<24)|(enc->sofar[1]<<16)|(enc->sofar[2]<<8)|enc->sofar[3];
    if ( val==0 ) {
	fputc('z',enc->sfd);
    } else {
	ch5 = val%85;
	val /= 85;
	ch4 = val%85;
	val /= 85;
	ch3 = val%85;
	val /= 85;
	ch2 = val%85;
	val /= 85;
	fputc('!'+val,enc->sfd);
	fputc('!'+ch2,enc->sfd);
	fputc('!'+ch3,enc->sfd);
	fputc('!'+ch4,enc->sfd);
	fputc('!'+ch5,enc->sfd);
    }
    enc->pos = 0;
}
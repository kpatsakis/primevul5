static void SFDEnc85(struct enc85 *enc,int ch) {
    enc->sofar[enc->pos++] = ch;
    if ( enc->pos==4 ) {
	unsigned int val = (enc->sofar[0]<<24)|(enc->sofar[1]<<16)|(enc->sofar[2]<<8)|enc->sofar[3];
	if ( val==0 ) {
	    fputc('z',enc->sfd);
	    ++enc->ccnt;
	} else {
	    int ch2, ch3, ch4, ch5;
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
	    enc->ccnt += 5;
	    if ( enc->ccnt > 70 ) { fputc('\n',enc->sfd); enc->ccnt=0; }
	}
	enc->pos = 0;
    }
}
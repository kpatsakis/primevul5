static void SFDGetTtInstrs(FILE *sfd, SplineChar *sc) {
    /* We've read the TtInstr token, it is followed by text versions of */
    /*  the instructions, slurp it all into a big buffer, and then parse that */
    char *buf=NULL, *pt=buf, *end=buf;
    int ch;
    int backlen = strlen(end_tt_instrs);
    int instr_len;

    while ( (ch=nlgetc(sfd))!=EOF ) {
	if ( pt>=end ) {
	    char *newbuf = realloc(buf,(end-buf+200));
	    pt = newbuf+(pt-buf);
	    end = newbuf+(end+200-buf);
	    buf = newbuf;
	}
	*pt++ = ch;
	if ( pt-buf>backlen && strncmp(pt-backlen,end_tt_instrs,backlen)==0 ) {
	    pt -= backlen;
    break;
	}
    }
    *pt = '\0';

    sc->ttf_instrs = _IVParse(sc->parent,buf,&instr_len,tterr,NULL);
    sc->ttf_instrs_len = instr_len;

    free(buf);
}
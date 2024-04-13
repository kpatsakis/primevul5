static void SFDDumpUTF7Str(FILE *sfd, const char *_str) {
    int ch, prev_cnt=0, prev=0, in=0;
    const unsigned char *str = (const unsigned char *) _str;

    putc('"',sfd);
    if ( str!=NULL ) while ( (ch = *str++)!='\0' ) {
	/* Convert from utf8 to ucs4 */
	if ( ch<=127 )
	    /* Done */;
	else if ( ch<=0xdf && *str!='\0' ) {
	    ch = ((ch&0x1f)<<6) | (*str++&0x3f);
	} else if ( ch<=0xef && *str!='\0' && str[1]!='\0' ) {
	    ch = ((ch&0xf)<<12) | ((str[0]&0x3f)<<6) | (str[1]&0x3f);
	    str += 2;
	} else if ( *str!='\0' && str[1]!='\0' && str[2]!='\0' ) {
	    int w = ( ((ch&0x7)<<2) | ((str[0]&0x30)>>4) )-1;
	    int s1, s2;
	    s1 = (w<<6) | ((str[0]&0xf)<<2) | ((str[1]&0x30)>>4);
	    s2 = ((str[1]&0xf)<<6) | (str[2]&0x3f);
	    ch = (s1*0x400)+s2 + 0x10000;
	    str += 3;
	} else {
	    /* illegal */
	}
	if ( ch<127 && ch!='\n' && ch!='\r' && ch!='\\' && ch!='~' &&
		ch!='+' && ch!='=' && ch!='"' ) {
	    if ( prev_cnt!=0 ) {
		prev<<= (prev_cnt==1?16:8);
		utf7_encode(sfd,prev);
		prev_cnt=prev=0;
	    }
	    if ( in ) {
		if ( inbase64[ch]!=-1 || ch=='-' )
		    putc('-',sfd);
		in = 0;
	    }
	    putc(ch,sfd);
	} else if ( ch=='+' && !in ) {
	    putc('+',sfd);
	    putc('-',sfd);
	} else if ( prev_cnt== 0 ) {
	    if ( !in ) {
		putc('+',sfd);
		in = 1;
	    }
	    prev = ch;
	    prev_cnt = 2;		/* 2 bytes */
	} else if ( prev_cnt==2 ) {
	    prev<<=8;
	    prev += (ch>>8)&0xff;
	    utf7_encode(sfd,prev);
	    prev = (ch&0xff);
	    prev_cnt=1;
	} else {
	    prev<<=16;
	    prev |= ch;
	    utf7_encode(sfd,prev);
	    prev_cnt = prev = 0;
	}
    }
    if ( prev_cnt==2 ) {
	prev<<=8;
	utf7_encode(sfd,prev);
    } else if ( prev_cnt==1 ) {
	prev<<=16;
	utf7_encode(sfd,prev);
    }
    putc('"',sfd);
}
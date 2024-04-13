static char *SFDReadUTF7Str(FILE *sfd) {
    char *buffer = NULL, *pt, *end = NULL;
    int ch1, ch2, ch3, ch4, done, c;
    int prev_cnt=0, prev=0, in=0;

    ch1 = nlgetc(sfd);
    while ( isspace(ch1) && ch1!='\n' && ch1!='\r') ch1 = nlgetc(sfd);
    if ( ch1=='\n' || ch1=='\r' )
	ungetc(ch1,sfd);
    if ( ch1!='"' )
return( NULL );
    pt = 0;
    while ( (ch1=nlgetc(sfd))!=EOF && ch1!='"' ) {
	done = 0;
	if ( !done && !in ) {
	    if ( ch1=='+' ) {
		ch1 = nlgetc(sfd);
		if ( ch1=='-' ) {
		    ch1 = '+';
		    done = true;
		} else {
		    in = true;
		    prev_cnt = 0;
		}
	    } else
		done = true;
	}
	if ( !done ) {
	    if ( ch1=='-' ) {
		in = false;
	    } else if ( inbase64[ch1]==-1 ) {
		in = false;
		done = true;
	    } else {
		ch1 = inbase64[ch1];
		ch2 = inbase64[c = nlgetc(sfd)];
		if ( ch2==-1 ) {
		    ungetc(c, sfd);
		    ch2 = ch3 = ch4 = 0;
		} else {
		    ch3 = inbase64[c = nlgetc(sfd)];
		    if ( ch3==-1 ) {
			ungetc(c, sfd);
			ch3 = ch4 = 0;
		    } else {
			ch4 = inbase64[c = nlgetc(sfd)];
			if ( ch4==-1 ) {
			    ungetc(c, sfd);
			    ch4 = 0;
			}
		    }
		}
		ch1 = (ch1<<18) | (ch2<<12) | (ch3<<6) | ch4;
		if ( prev_cnt==0 ) {
		    prev = ch1&0xff;
		    ch1 >>= 8;
		    prev_cnt = 1;
		} else /* if ( prev_cnt == 1 ) */ {
		    ch1 |= (prev<<24);
		    prev = (ch1&0xffff);
		    ch1 = (ch1>>16)&0xffff;
		    prev_cnt = 2;
		}
		done = true;
	    }
	}
	if ( pt+10>=end ) {
	    if ( buffer==NULL ) {
		pt = buffer = malloc(400);
		end = buffer+400;
	    } else if (pt) {
		char *temp = realloc(buffer,end-buffer+400);
		pt = temp+(pt-buffer);
		end = temp+(end-buffer+400);
		buffer = temp;
	    }
	}
	if ( pt && done )
	    pt = utf8_idpb(pt,ch1,0);
	if ( prev_cnt==2 ) {
	    prev_cnt = 0;
	    if ( pt && prev!=0 )
		pt = utf8_idpb(pt,prev,0);
	}
	if ( pt==0 ) {
	    free(buffer);
	    return( NULL );
	}
    }
    if ( buffer==NULL )
return( NULL );
    *pt = '\0';
    pt = copy(buffer);
    free(buffer );
return( pt );
}
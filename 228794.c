static ValDevTab *SFDReadValDevTab(FILE *sfd) {
    int i, j, ch;
    ValDevTab vdt;
    char buf[4];

    memset(&vdt,0,sizeof(vdt));
    buf[3] = '\0';
    while ( (ch=nlgetc(sfd))==' ' );
    if ( ch=='[' ) {
	for ( i=0; i<4; ++i ) {
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch==']' )
	break;
	    buf[0]=ch;
	    for ( j=1; j<3; ++j ) buf[j]=nlgetc(sfd);
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch!='=' ) ungetc(ch,sfd);
	    SFDReadDeviceTable(sfd,
		    strcmp(buf,"ddx")==0 ? &vdt.xadjust :
		    strcmp(buf,"ddy")==0 ? &vdt.yadjust :
		    strcmp(buf,"ddh")==0 ? &vdt.xadv :
		    strcmp(buf,"ddv")==0 ? &vdt.yadv :
			(&vdt.xadjust) + i );
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch!=']' ) ungetc(ch,sfd);
	    else
	break;
	}
	if ( vdt.xadjust.corrections!=NULL || vdt.yadjust.corrections!=NULL ||
		vdt.xadv.corrections!=NULL || vdt.yadv.corrections!=NULL ) {
	    ValDevTab *v = chunkalloc(sizeof(ValDevTab));
	    *v = vdt;
return( v );
	}
    } else
	ungetc(ch,sfd);
return( NULL );
}
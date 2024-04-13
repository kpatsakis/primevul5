void visitSFDFragment( FILE *sfd, SplineFont *sf,
		       visitSFDFragmentFunc ufunc, void* udata )
{
    int eof;
    char tok[2000];
    while ( 1 ) {
	if ( (eof = getname(sfd,tok))!=1 ) {
	    if ( eof==-1 )
		break;
	    geteol(sfd,tok);
	    continue;
	}

	ufunc( sfd, tok, sf, udata );
    }
}
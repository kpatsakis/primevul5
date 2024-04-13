static int SLContains(struct script_record *sr, uint32 script, uint32 lang) {
    int i, j;

    if ( script==DEFAULT_SCRIPT || script == 0 )
return( true );
    for ( i=0; sr[i].script!=0; ++i ) {
	if ( sr[i].script==script ) {
	    if ( lang==0 )
return( true );
	    for ( j=0; sr[i].langs[j]!=0; ++j )
		if ( sr[i].langs[j]==lang )
return( true );

return( false );	/* this script entry didn't contain the language. won't be any other scripts to check */
	}
    }
return( false );	/* Never found script */
}
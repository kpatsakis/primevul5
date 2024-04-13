static int SLCount(struct script_record *sr) {
    int sl_cnt = 0;
    int i,j;

    for ( i=0; sr[i].script!=0; ++i ) {
	for ( j=0; sr[i].langs[j]!=0; ++j )
	    ++sl_cnt;
    }
return( sl_cnt );
}
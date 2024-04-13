static void CvtOldMacFeature(PST1 *pst) {
    int i;

    if ( pst->macfeature )
return;
    for ( i=0; formertags[i].feature!=-1 ; ++i ) {
	if ( pst->tag == formertags[i].tag ) {
	    pst->macfeature = true;
	    pst->tag = (formertags[i].feature<<16) | formertags[i].setting;
return;
	}
    }
}
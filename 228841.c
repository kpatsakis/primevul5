static DStemInfo *SFDReadDHints( SplineFont *sf,FILE *sfd,int old ) {
    DStemInfo *head=NULL, *last=NULL, *cur;
    int i;
    BasePoint bp[4], *bpref[4], left, right, unit;
    double rstartoff, rendoff, lendoff;

    if ( old ) {
        for ( i=0 ; i<4 ; i++ ) bpref[i] = &bp[i];

        while ( getreal( sfd,&bp[0].x ) && getreal( sfd,&bp[0].y ) &&
	        getreal( sfd,&bp[1].x ) && getreal( sfd,&bp[1].y ) &&
	        getreal( sfd,&bp[2].x ) && getreal( sfd,&bp[2].y ) &&
	        getreal( sfd,&bp[3].x ) && getreal( sfd,&bp[3].y )) {

            /* Ensure point coordinates specified in the sfd file do */
            /* form a diagonal line */
            if ( PointsDiagonalable( sf,bpref,&unit )) {
	        cur = chunkalloc( sizeof( DStemInfo ));
	        cur->left = *bpref[0];
	        cur->right = *bpref[1];
                cur->unit = unit;
                /* Generate a temporary hint instance, so that the hint can */
                /* be visible in charview even if subsequent rebuilding instances */
                /* fails (e. g. for composite characters) */
                cur->where = chunkalloc( sizeof( HintInstance ));
                rstartoff = ( cur->right.x - cur->left.x ) * cur->unit.x +
                            ( cur->right.y - cur->left.y ) * cur->unit.y;
                rendoff =   ( bpref[2]->x - cur->left.x ) * cur->unit.x +
                            ( bpref[2]->y - cur->left.y ) * cur->unit.y;
                lendoff =   ( bpref[3]->x - cur->left.x ) * cur->unit.x +
                            ( bpref[3]->y - cur->left.y ) * cur->unit.y;
                cur->where->begin = ( rstartoff > 0 ) ? rstartoff : 0;
                cur->where->end   = ( rendoff > lendoff ) ? lendoff : rendoff;
                MergeDStemInfo( sf,&head,cur );
            }
        }
    } else {
        while ( getreal( sfd,&left.x ) && getreal( sfd,&left.y ) &&
                getreal( sfd,&right.x ) && getreal( sfd,&right.y ) &&
                getreal( sfd,&unit.x ) && getreal( sfd,&unit.y )) {
	    cur = chunkalloc( sizeof( DStemInfo ));
	    cur->left = left;
	    cur->right = right;
            cur->unit = unit;
	    cur->where = SFDReadHintInstances( sfd,NULL );
	    if ( head == NULL )
	        head = cur;
	    else
	        last->next = cur;
	    last = cur;
        }
    }
return( head );
}
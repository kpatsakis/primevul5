static int SFDCloseCheck(SplinePointList *spl,int order2) {
    if ( spl->first!=spl->last &&
	    RealNear(spl->first->me.x,spl->last->me.x) &&
	    RealNear(spl->first->me.y,spl->last->me.y)) {
	SplinePoint *oldlast = spl->last;
	spl->first->prevcp = oldlast->prevcp;
	spl->first->noprevcp = oldlast->noprevcp;
	oldlast->prev->from->next = NULL;
	spl->last = oldlast->prev->from;
	chunkfree(oldlast->prev,sizeof(*oldlast));
	chunkfree(oldlast->hintmask,sizeof(HintMask));
	chunkfree(oldlast,sizeof(*oldlast));
	SplineMake(spl->last,spl->first,order2);
	spl->last = spl->first;
return( true );
    }
return( false );
}
static AnchorPoint *SFDReadAnchorPoints(FILE *sfd,SplineChar *sc,AnchorPoint** alist, AnchorPoint *lastap)
{
    AnchorPoint *ap = chunkalloc(sizeof(AnchorPoint));
    AnchorClass *an;
    char *name;
    char tok[200];
    int ch;

    name = SFDReadUTF7Str(sfd);
    if ( name==NULL ) {
	LogError(_("Anchor Point with no class name: %s"), sc->name );
	AnchorPointsFree(ap);
return( lastap );
    }
    for ( an=sc->parent->anchor; an!=NULL && strcmp(an->name,name)!=0; an=an->next );
    free(name);
    ap->anchor = an;
    getreal(sfd,&ap->me.x);
    getreal(sfd,&ap->me.y);
    ap->type = -1;
    if ( getname(sfd,tok)==1 ) {
	if ( strcmp(tok,"mark")==0 )
	    ap->type = at_mark;
	else if ( strcmp(tok,"basechar")==0 )
	    ap->type = at_basechar;
	else if ( strcmp(tok,"baselig")==0 )
	    ap->type = at_baselig;
	else if ( strcmp(tok,"basemark")==0 )
	    ap->type = at_basemark;
	else if ( strcmp(tok,"entry")==0 )
	    ap->type = at_centry;
	else if ( strcmp(tok,"exit")==0 )
	    ap->type = at_cexit;
    }
    getsint(sfd,&ap->lig_index);
    ch = nlgetc(sfd);
    ungetc(ch,sfd);
    if ( ch==' ' ) {
	SFDReadDeviceTable(sfd,&ap->xadjust);
	SFDReadDeviceTable(sfd,&ap->yadjust);
	ch = nlgetc(sfd);
	ungetc(ch,sfd);
	if ( isdigit(ch)) {
	    getsint(sfd,(int16 *) &ap->ttf_pt_index);
	    ap->has_ttf_pt = true;
	}
    }
    if ( ap->anchor==NULL || ap->type==-1 ) {
	LogError(_("Bad Anchor Point: %s"), sc->name );
	AnchorPointsFree(ap);
return( lastap );
    }
    if ( lastap==NULL )
	(*alist) = ap;
    else
	lastap->next = ap;

    return( ap );
}
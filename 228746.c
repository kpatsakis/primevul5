static OTLookup *SFD_ParseNestedLookup(FILE *sfd, SplineFont *sf, int old) {
    uint32 tag;
    int ch, isgpos;
    OTLookup *otl;
    char *name;

    while ( (ch=nlgetc(sfd))==' ' );
    if ( ch=='~' )
return( NULL );
    else if ( old ) {
	if ( ch!='\'' )
return( NULL );

	ungetc(ch,sfd);
	tag = gettag(sfd);
return( (OTLookup *) (intpt) tag );
    } else {
	ungetc(ch,sfd);
	name = SFDReadUTF7Str(sfd);
	if ( name==NULL )
return( NULL );
	for ( isgpos=0; isgpos<2; ++isgpos ) {
	    for ( otl=isgpos ? sf->gpos_lookups : sf->gsub_lookups; otl!=NULL; otl=otl->next ) {
		if ( strcmp(name,otl->lookup_name )==0 )
	goto break2;
	    }
	}
	break2:
	free(name);
return( otl );
    }
}
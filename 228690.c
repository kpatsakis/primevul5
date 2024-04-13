void SFD_DumpLookup( FILE *sfd, SplineFont *sf ) {
    int isgpos;
    OTLookup *otl;
    struct lookup_subtable *sub;
    FeatureScriptLangList *fl;
    struct scriptlanglist *sl;
    int i;

    for ( isgpos=0; isgpos<2; ++isgpos ) {
	for ( otl = isgpos ? sf->gpos_lookups : sf->gsub_lookups; otl!=NULL; otl = otl->next ) {
	    fprintf( sfd, "Lookup: %d %d %d ", otl->lookup_type, otl->lookup_flags, otl->store_in_afm );
	    SFDDumpUTF7Str(sfd,otl->lookup_name);
	    fprintf( sfd, " { " );
	    for ( sub=otl->subtables; sub!=NULL; sub=sub->next ) {
		SFDDumpUTF7Str(sfd,sub->subtable_name);
		putc(' ',sfd);
		if ( otl->lookup_type==gsub_single && sub->suffix!=NULL ) {
		    putc('(',sfd);
		    SFDDumpUTF7Str(sfd,sub->suffix);
		    putc(')',sfd);
		} else if ( otl->lookup_type==gpos_pair && sub->vertical_kerning )
		    fprintf(sfd,"(1)");
		if ( otl->lookup_type==gpos_pair && (sub->separation!=0 || sub->kerning_by_touch))
		    fprintf(sfd,"[%d,%d,%d]", sub->separation, sub->minkern, sub->kerning_by_touch+2*sub->onlyCloser+4*sub->dontautokern );
		putc(' ',sfd);
	    }
	    fprintf( sfd, "} [" );
	    for ( fl=otl->features; fl!=NULL; fl=fl->next ) {
		if ( fl->ismac )
		    fprintf( sfd, "<%d,%d> (",
			    (int) (fl->featuretag>>16),
			    (int) (fl->featuretag&0xffff));
		else
		    fprintf( sfd, "'%c%c%c%c' (",
			    (int) (fl->featuretag>>24), (int) ((fl->featuretag>>16)&0xff),
			    (int) ((fl->featuretag>>8)&0xff), (int) (fl->featuretag&0xff) );
		for ( sl= fl->scripts; sl!=NULL; sl = sl->next ) {
		    fprintf( sfd, "'%c%c%c%c' <",
			    (int) (sl->script>>24), (int) ((sl->script>>16)&0xff),
			    (int) ((sl->script>>8)&0xff), (int) (sl->script&0xff) );
		    for ( i=0; i<sl->lang_cnt; ++i ) {
			uint32 lang = i<MAX_LANG ? sl->langs[i] : sl->morelangs[i-MAX_LANG];
			fprintf( sfd, "'%c%c%c%c' ",
				(int) (lang>>24), (int) ((lang>>16)&0xff),
				(int) ((lang>>8)&0xff), (int) (lang&0xff) );
		    }
		    fprintf( sfd, "> " );
		}
		fprintf( sfd, ") " );
	    }
	    fprintf( sfd, "]\n" );
	}
    }
}
static void ACDisassociateLigatures(SplineFont1 *sf,AnchorClass1 *ac) {
    int gid,k;
    SplineFont1 *subsf;
    SplineChar *sc;
    AnchorPoint *ap, *lap;
    AnchorClass1 *lac;
    char *format;

    lac = chunkalloc(sizeof(AnchorClass1));
    *lac = *ac;
    lac->ac.type = act_mklg;
    ac->ac.next = (AnchorClass *) lac;

  /* GT: Need to split some AnchorClasses into two classes, one for normal */
  /* GT:  base letters, and one for ligatures. So create a new AnchorClass */
  /* GT:  name for the ligature version */
    format = _("Ligature %s");
    lac->ac.name = malloc(strlen(ac->ac.name)+strlen(format)+1);
    sprintf( lac->ac.name, format, ac->ac.name );

    k=0;
    do {
	subsf = sf->sf.subfontcnt==0 ? sf : (SplineFont1 *) (sf->sf.subfonts[k]);
	for ( gid=0; gid<subsf->sf.glyphcnt; ++gid ) if ( (sc=subsf->sf.glyphs[gid])!=NULL ) {
	    for ( ap=sc->anchor; ap!=NULL; ap=ap->next ) {
		if ( ap->anchor!=(AnchorClass *) ac )
	    continue;
		if ( ap->type==at_mark ) {
		    lap = chunkalloc(sizeof(AnchorPoint));
		    *lap = *ap;
		    ap->next = lap;
		    lap->anchor = (AnchorClass *) lac;
		} else if ( ap->type==at_baselig ) {
		    ap->anchor = (AnchorClass *) lac;
		}
	    }
	}
	++k;
    } while ( k<sf->sf.subfontcnt );
}
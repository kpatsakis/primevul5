static struct lookup_subtable *CreateSubtable(OTLookup *otl,SplineFont1 *sf) {
    struct lookup_subtable *cur, *prev;

    cur = chunkalloc(sizeof(struct lookup_subtable));
    if ( otl->subtables==NULL )
	otl->subtables = cur;
    else {
	for ( prev=otl->subtables; prev->next!=NULL; prev=prev->next );
	prev->next = cur;
    }
    cur->lookup = otl;
    if ( otl->lookup_type == gsub_single ||
	    otl->lookup_type == gsub_multiple ||
	    otl->lookup_type == gsub_alternate ||
	    otl->lookup_type == gsub_ligature ||
	    otl->lookup_type == gpos_single ||
	    otl->lookup_type == gpos_pair )
	cur->per_glyph_pst_or_kern = true;
    else if ( otl->lookup_type == gpos_cursive ||
	    otl->lookup_type == gpos_mark2base ||
	    otl->lookup_type == gpos_mark2ligature ||
	    otl->lookup_type == gpos_mark2mark )
	cur->anchor_classes = true;
    if ( otl->lookup_type == gpos_pair ) {
	if ( otl->features!=NULL &&
		otl->features->featuretag==CHR('v','k','r','n'))
	    cur->vertical_kerning = true;
    }
return( cur );
}
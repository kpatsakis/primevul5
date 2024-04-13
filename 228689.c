static int GSubOrder(SplineFont1 *sf,FeatureScriptLangList *fl) {
    struct table_ordering *ord;
    int sofar = 30000, temp;

    for ( ord=sf->orders; ord!=NULL && ord->table_tag!=CHR('G','S','U','B');
	    ord = ord->next );
    for ( ; fl!=NULL; fl=fl->next ) {
	temp = TTFFeatureIndex(fl->featuretag,ord);
	if ( temp<sofar )
	    sofar = temp;
    }
return( sofar );
}
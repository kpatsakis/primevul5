static PST1 *LigaCreateFromOldStyleMultiple(PST1 *liga) {
    char *pt;
    PST1 *new, *last=liga;
    while ( (pt = strrchr(liga->pst.u.lig.components,';'))!=NULL ) {
	new = chunkalloc(sizeof( PST1 ));
	*new = *liga;
	new->pst.u.lig.components = copy(pt+1);
	last->pst.next = (PST *) new;
	last = new;
	*pt = '\0';
    }
return( last );
}
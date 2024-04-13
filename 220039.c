Header headerReload(Header h, rpmTagVal tag)
{
    Header nh;
    unsigned int uc = 0;
    void * uh = headerExport(h, &uc);

    h = headerFree(h);
    if (uh == NULL)
	return NULL;
    nh = headerImport(uh, uc, 0);
    if (nh == NULL) {
	uh = _free(uh);
	return NULL;
    }
    if (ENTRY_IS_REGION(nh->index)) {
	if (tag == RPMTAG_HEADERSIGNATURES || tag == RPMTAG_HEADERIMMUTABLE)
	    nh->index[0].info.tag = tag;
    }
    return nh;
}
static rpmRC hdrblobVerifyInfo(hdrblob blob, char **emsg)
{
    struct entryInfo_s info;
    int i, len = 0;
    int32_t end = 0;
    const char *ds = (const char *) blob->dataStart;
    int32_t il = (blob->regionTag) ? blob->il-1 : blob->il;
    entryInfo pe = (blob->regionTag) ? blob->pe+1 : blob->pe;
    /* Can't typecheck signature header tags, sigh */
    int typechk = (blob->regionTag == RPMTAG_HEADERIMMUTABLE ||
		   blob->regionTag == RPMTAG_HEADERIMAGE);

    for (i = 0; i < il; i++) {
	ei2h(&pe[i], &info);

	/* Previous data must not overlap */
	if (end > info.offset)
	    goto err;

	if (hdrchkTag(info.tag))
	    goto err;
	if (hdrchkType(info.type))
	    goto err;
	if (hdrchkCount(info.count))
	    goto err;
	if (hdrchkAlign(info.type, info.offset))
	    goto err;
	if (hdrchkRange(blob->dl, info.offset))
	    goto err;
	if (typechk && hdrchkTagType(info.tag, info.type))
	    goto err;

	/* Verify the data actually fits */
	len = dataLength(info.type, ds + info.offset,
			 info.count, 1, ds + blob->dl);
	if (hdrchkRange(blob->dl - info.offset, len))
	    goto err;
	end = info.offset + len;
	if (blob->regionTag) {
	    /*
	     * Verify that the data does not overlap the region trailer.  The
	     * region trailer is skipped by this loop, so the other checks
	     * don’t catch this case.
	     */
	    if (end > blob->rdl - REGION_TAG_COUNT && info.offset < blob->rdl)
		goto err;
	}
    }
    return 0; /* Everything ok */

err:
    if (emsg) {
	rasprintf(emsg,
		  _("tag[%d]: BAD, tag %d type %d offset %d count %d len %d"),
		    i, info.tag, info.type, info.offset, info.count, len);
    }
    return i + 1;
}
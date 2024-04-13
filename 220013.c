static rpmRC hdrblobVerifyRegion(rpmTagVal regionTag, int exact_size,
			hdrblob blob, char **buf)
{
    rpmRC rc = RPMRC_FAIL;
    struct entryInfo_s trailer, einfo;
    unsigned char * regionEnd = NULL;

    /* Check that we have at least on tag */
    if (blob->il < 1) {
	rasprintf(buf, _("region: no tags"));
	goto exit;
    }

    /* Convert the 1st tag element. */
    ei2h(blob->pe, &einfo);

    if (!regionTag && (einfo.tag == RPMTAG_HEADERSIGNATURES ||
		       einfo.tag == RPMTAG_HEADERIMMUTABLE ||
		       einfo.tag == RPMTAG_HEADERIMAGE)) {
	regionTag = einfo.tag;
    }

    /* Is there an immutable header region tag? */
    if (!(einfo.tag == regionTag)) {
	rc = RPMRC_NOTFOUND;
	goto exit;
    }

    /* Is the region tag sane? */
    if (!(einfo.type == REGION_TAG_TYPE && einfo.count == REGION_TAG_COUNT)) {
	rasprintf(buf,
		_("region tag: BAD, tag %d type %d offset %d count %d"),
		einfo.tag, einfo.type, einfo.offset, einfo.count);
	goto exit;
    }

    /* Is the trailer within the data area? */
    if (hdrchkRange(blob->dl, einfo.offset + REGION_TAG_COUNT)) {
	rasprintf(buf,
		_("region offset: BAD, tag %d type %d offset %d count %d"),
		einfo.tag, einfo.type, einfo.offset, einfo.count);
	goto exit;
    }

    /* Is there an immutable header region tag trailer? */
    memset(&trailer, 0, sizeof(trailer));
    regionEnd = blob->dataStart + einfo.offset;
    /* regionEnd is not guaranteed to be aligned */
    (void) memcpy(&trailer, regionEnd, REGION_TAG_COUNT);
    blob->rdl = einfo.offset + REGION_TAG_COUNT;

    ei2h(&trailer, &einfo);
    /* Some old packages have HEADERIMAGE in signature region trailer, fix up */
    if (regionTag == RPMTAG_HEADERSIGNATURES && einfo.tag == RPMTAG_HEADERIMAGE)
	einfo.tag = RPMTAG_HEADERSIGNATURES;
    if (!(einfo.tag == regionTag &&
	  einfo.type == REGION_TAG_TYPE && einfo.count == REGION_TAG_COUNT))
    {
	rasprintf(buf,
		_("region trailer: BAD, tag %d type %d offset %d count %d"),
		einfo.tag, einfo.type, einfo.offset, einfo.count);
	goto exit;
    }

    /*
     * Trailer offset is negative and has a special meaning.  Be sure to negate
     * *after* the division, so the negation cannot overflow.  The parentheses
     * around the division are required!
     *
     * Thankfully, the modulus operator works fine on negative numbers.
     */
    blob->ril = -(einfo.offset/sizeof(*blob->pe));
    /* Does the region actually fit within the header? */
    if ((einfo.offset % sizeof(*blob->pe)) || hdrchkRange(blob->il, blob->ril) ||
					hdrchkRange(blob->dl, blob->rdl)) {
	rasprintf(buf, _("region %d size: BAD, ril %d il %d rdl %d dl %d"),
			regionTag, blob->ril, blob->il, blob->rdl, blob->dl);
	goto exit;
    }

    /* In package files region size is expected to match header size. */
    if (exact_size && !(blob->il == blob->ril && blob->dl == blob->rdl)) {
	rasprintf(buf,
		_("region %d: tag number mismatch il %d ril %d dl %d rdl %d\n"),
		regionTag, blob->il, blob->ril, blob->dl, blob->rdl);
	goto exit;
    }

    blob->regionTag = regionTag;
    rc = RPMRC_OK;

exit:
    return rc;
}
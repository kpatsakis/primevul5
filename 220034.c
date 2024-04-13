static int regionSwab(indexEntry entry, int il, int dl,
		entryInfo pe,
		unsigned char * dataStart,
		const unsigned char * dataEnd,
		int regionid, int fast)
{
    if ((entry != NULL && regionid >= 0) || (entry == NULL && regionid != 0))
	return -1;

    for (; il > 0; il--, pe++) {
	struct indexEntry_s ie;

	ei2h(pe, &ie.info);

	if (hdrchkType(ie.info.type))
	    return -1;
	if (hdrchkData(ie.info.count))
	    return -1;
	if (hdrchkData(ie.info.offset))
	    return -1;
	if (hdrchkAlign(ie.info.type, ie.info.offset))
	    return -1;

	ie.data = dataStart + ie.info.offset;
	if (dataEnd && (unsigned char *)ie.data >= dataEnd)
	    return -1;

	/* The offset optimization is only relevant for string types */
	if (fast && il > 1 && typeSizes[ie.info.type] == -1) {
	    ie.length = ntohl(pe[1].offset) - ie.info.offset;
	} else {
	    ie.length = dataLength(ie.info.type, ie.data, ie.info.count,
				   1, dataEnd);
	}
	if (ie.length < 0 || hdrchkData(ie.length))
	    return -1;

	ie.rdlen = 0;

	if (entry) {
	    ie.info.offset = regionid;
	    *entry = ie;	/* structure assignment */
	    entry++;
	}

	/* Alignment */
	dl += alignDiff(ie.info.type, dl);

	/* Perform endian conversions */
	switch (ntohl(pe->type)) {
	case RPM_INT64_TYPE:
	{   uint64_t * it = ie.data;
	    for (; ie.info.count > 0; ie.info.count--, it += 1) {
		if (dataEnd && ((unsigned char *)it) >= dataEnd)
		    return -1;
		*it = htonll(*it);
	    }
	}   break;
	case RPM_INT32_TYPE:
	{   int32_t * it = ie.data;
	    for (; ie.info.count > 0; ie.info.count--, it += 1) {
		if (dataEnd && ((unsigned char *)it) >= dataEnd)
		    return -1;
		*it = htonl(*it);
	    }
	}   break;
	case RPM_INT16_TYPE:
	{   int16_t * it = ie.data;
	    for (; ie.info.count > 0; ie.info.count--, it += 1) {
		if (dataEnd && ((unsigned char *)it) >= dataEnd)
		    return -1;
		*it = htons(*it);
	    }
	}   break;
	}

	dl += ie.length;
    }

    return dl;
}
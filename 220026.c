static void * doExport(const struct indexEntry_s *hindex, int indexUsed,
			headerFlags flags, unsigned int *bsize)
{
    int32_t * ei = NULL;
    entryInfo pe;
    char * dataStart;
    char * te;
    unsigned len, diff;
    int32_t il = 0;
    int32_t dl = 0;
    indexEntry entry; 
    int i;
    int drlen, ndribbles;
    size_t ilen = indexUsed * sizeof(struct indexEntry_s);
    indexEntry index = memcpy(xmalloc(ilen), hindex, ilen);

    /* Sort entries by (offset,tag). */
    qsort(index, indexUsed, sizeof(*index), offsetCmp);

    /* Compute (il,dl) for all tags, including those deleted in region. */
    drlen = ndribbles = 0;
    for (i = 0, entry = index; i < indexUsed; i++, entry++) {
	if (ENTRY_IS_REGION(entry)) {
	    int32_t rdl = -entry->info.offset;	/* negative offset */
	    int32_t ril = rdl/sizeof(*pe);
	    int rid = entry->info.offset;

	    il += ril;
	    dl += entry->rdlen + entry->info.count;
	    /* Reserve space for legacy region tag */
	    if (i == 0 && (flags & HEADERFLAG_LEGACY))
		il += 1;

	    /* Skip rest of entries in region, but account for dribbles. */
	    for (; i < indexUsed && entry->info.offset <= rid+1; i++, entry++) {
		if (entry->info.offset <= rid)
		    continue;

		/* Alignment */
		diff = alignDiff(entry->info.type, dl);
		if (diff) {
		    drlen += diff;
		    dl += diff;    
		}

		ndribbles++;
		il++;
		drlen += entry->length;
		dl += entry->length;
	    }
	    i--;
	    entry--;
	    continue;
	}

	/* Ignore deleted drips. */
	if (entry->data == NULL || entry->length <= 0)
	    continue;

	/* Alignment */
	dl += alignDiff(entry->info.type, dl);

	il++;
	dl += entry->length;
    }

    /* Sanity checks on header intro. */
    if (hdrchkTags(il) || hdrchkData(dl))
	goto errxit;

    len = sizeof(il) + sizeof(dl) + (il * sizeof(*pe)) + dl;

    ei = xmalloc(len);
    ei[0] = htonl(il);
    ei[1] = htonl(dl);

    pe = (entryInfo) &ei[2];
    dataStart = te = (char *) (pe + il);

    for (i = 0, entry = index; i < indexUsed; i++, entry++) {
	const char * src;
	unsigned char *t;
	int count;
	int rdlen;
	unsigned int diff;

	if (entry->data == NULL || entry->length <= 0)
	    continue;

	t = (unsigned char*)te;
	pe->tag = htonl(entry->info.tag);
	pe->type = htonl(entry->info.type);
	pe->count = htonl(entry->info.count);

	if (ENTRY_IS_REGION(entry)) {
	    int32_t rdl = -entry->info.offset;	/* negative offset */
	    int32_t ril = rdl/sizeof(*pe) + ndribbles;
	    int rid = entry->info.offset;

	    src = (char *)entry->data;
	    rdlen = entry->rdlen;

	    /* Legacy headers don't have regions originally, create one */
	    if (i == 0 && (flags & HEADERFLAG_LEGACY)) {
		int32_t stei[4];

		memcpy(pe+1, src, rdl);
		memcpy(te, src + rdl, rdlen);
		te += rdlen;

		pe->offset = htonl(te - dataStart);
		stei[0] = pe->tag;
		stei[1] = pe->type;
		stei[2] = htonl(-rdl-entry->info.count);
		stei[3] = pe->count;
		memcpy(te, stei, entry->info.count);
		te += entry->info.count;
		ril++;
		rdlen += entry->info.count;

		count = regionSwab(NULL, ril, 0, pe, t, NULL, 0, 0);
		if (count != rdlen)
		    goto errxit;

	    } else {

		memcpy(pe+1, src + sizeof(*pe), ((ril-1) * sizeof(*pe)));
		memcpy(te, src + (ril * sizeof(*pe)), rdlen+entry->info.count+drlen);
		te += rdlen;
		{  
		    entryInfo se = (entryInfo)src;
		    int off = ntohl(se->offset);
		    pe->offset = (off) ? htonl(te - dataStart) : htonl(off);
		}
		te += entry->info.count + drlen;

		count = regionSwab(NULL, ril, 0, pe, t, NULL, 0, 0);
		if (count != (rdlen + entry->info.count + drlen))
		    goto errxit;
	    }

	    /* Skip rest of entries in region. */
	    while (i < indexUsed && entry->info.offset <= rid+1) {
		i++;
		entry++;
	    }
	    i--;
	    entry--;
	    pe += ril;
	    continue;
	}

	/* Ignore deleted drips. */
	if (entry->data == NULL || entry->length <= 0)
	    continue;

	/* Alignment */
	diff = alignDiff(entry->info.type, (te - dataStart));
	if (diff) {
	    memset(te, 0, diff);
	    te += diff;
	}

	pe->offset = htonl(te - dataStart);

	/* copy data w/ endian conversions */
	switch (entry->info.type) {
	case RPM_INT64_TYPE:
	    count = entry->info.count;
	    src = entry->data;
	    while (count--) {
		*((uint64_t *)te) = htonll(*((uint64_t *)src));
		te += sizeof(uint64_t);
		src += sizeof(uint64_t);
	    }
	    break;

	case RPM_INT32_TYPE:
	    count = entry->info.count;
	    src = entry->data;
	    while (count--) {
		*((int32_t *)te) = htonl(*((int32_t *)src));
		te += sizeof(int32_t);
		src += sizeof(int32_t);
	    }
	    break;

	case RPM_INT16_TYPE:
	    count = entry->info.count;
	    src = entry->data;
	    while (count--) {
		*((int16_t *)te) = htons(*((int16_t *)src));
		te += sizeof(int16_t);
		src += sizeof(int16_t);
	    }
	    break;

	default:
	    memcpy(te, entry->data, entry->length);
	    te += entry->length;
	    break;
	}
	pe++;
    }
   
    /* Insure that there are no memcpy underruns/overruns. */
    if (((char *)pe) != dataStart)
	goto errxit;
    if ((((char *)ei)+len) != te)
	goto errxit;

    if (bsize)
	*bsize = len;

    free(index);
    return (void *) ei;

errxit:
    free(ei);
    free(index);
    return NULL;
}
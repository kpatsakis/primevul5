rpmRC hdrblobImport(hdrblob blob, int fast, Header *hdrp, char **emsg)
{
    Header h = NULL;
    indexEntry entry; 
    int rdlen;

    h = headerCreate(blob->ei, blob->il);

    entry = h->index;
    if (!(htonl(blob->pe->tag) < RPMTAG_HEADERI18NTABLE)) {
	/* An original v3 header, create a legacy region entry for it */
	h->flags |= HEADERFLAG_LEGACY;
	entry->info.type = REGION_TAG_TYPE;
	entry->info.tag = RPMTAG_HEADERIMAGE;
	entry->info.count = REGION_TAG_COUNT;
	entry->info.offset = ((unsigned char *)blob->pe - blob->dataStart); /* negative offset */

	entry->data = blob->pe;
	entry->length = blob->pvlen - sizeof(blob->il) - sizeof(blob->dl);
	rdlen = regionSwab(entry+1, blob->il, 0, blob->pe,
			   blob->dataStart, blob->dataEnd,
			   entry->info.offset, fast);
	if (rdlen != blob->dl)
	    goto errxit;
	entry->rdlen = rdlen;
	h->indexUsed++;
    } else {
	/* Either a v4 header or an "upgraded" v3 header with a legacy region */
	int32_t ril;

	h->flags &= ~HEADERFLAG_LEGACY;
	ei2h(blob->pe, &entry->info);
	ril = (entry->info.offset != 0) ? blob->ril : blob->il;

	entry->info.offset = -(ril * sizeof(*blob->pe)); /* negative offset */
	entry->data = blob->pe;
	entry->length = blob->pvlen - sizeof(blob->il) - sizeof(blob->dl);
	rdlen = regionSwab(entry+1, ril-1, 0, blob->pe+1,
			   blob->dataStart, blob->dataEnd,
			   entry->info.offset, fast);
	if (rdlen < 0)
	    goto errxit;
	entry->rdlen = rdlen;

	if (ril < h->indexUsed) {
	    indexEntry newEntry = entry + ril;
	    int ne = (h->indexUsed - ril);
	    int rid = entry->info.offset+1;

	    /* Load dribble entries from region. */
	    rdlen = regionSwab(newEntry, ne, rdlen, blob->pe+ril,
				blob->dataStart, blob->dataEnd, rid, fast);
	    if (rdlen < 0)
		goto errxit;

	  { indexEntry firstEntry = newEntry;
	    int save = h->indexUsed;
	    int j;

	    /* Dribble entries replace duplicate region entries. */
	    h->indexUsed -= ne;
	    for (j = 0; j < ne; j++, newEntry++) {
		(void) headerDel(h, newEntry->info.tag);
		if (newEntry->info.tag == RPMTAG_BASENAMES)
		    (void) headerDel(h, RPMTAG_OLDFILENAMES);
	    }

	    /* If any duplicate entries were replaced, move new entries down. */
	    if (h->indexUsed < (save - ne)) {
		memmove(h->index + h->indexUsed, firstEntry,
			(ne * sizeof(*entry)));
	    }
	    h->indexUsed += ne;
	  }
	}

	rdlen += REGION_TAG_COUNT;

	if (rdlen != blob->dl)
	    goto errxit;
    }

    /* Force sorting, dribble lookups can cause early sort on partial header */
    h->sorted = 0;
    headerSort(h);
    h->flags |= HEADERFLAG_ALLOCATED;
    *hdrp = h;

    /* We own the memory now, avoid double-frees */
    blob->ei = NULL;

    return RPMRC_OK;

errxit:
    if (h) {
	free(h->index);
	free(h);
	rasprintf(emsg, _("hdr load: BAD"));
    }
    return RPMRC_FAIL;
}
ldns_rdf_bitmap_known_rr_types_set(ldns_rdf** rdf, int value)
{
	uint8_t  window;		/*  most significant octet of type */
	uint8_t  subtype;		/* least significant octet of type */
	uint16_t windows[256]		/* Max subtype per window */
#ifndef S_SPLINT_S
	                      = { 0 }
#endif
	                             ;
	ldns_rr_descriptor* d;	/* used to traverse rdata_field_descriptors */
	size_t i;		/* used to traverse windows array */

	size_t sz;			/* size needed for type bitmap rdf */
	uint8_t* data = NULL;		/* rdf data */
	uint8_t* dptr;			/* used to itraverse rdf data */

	assert(rdf != NULL);

	/* Which windows need to be in the bitmap rdf?
	 */
	for (d=rdata_field_descriptors; d < rdata_field_descriptors_end; d++) {
		window  = d->_type >> 8;
		subtype = d->_type & 0xff;
		if (windows[window] < subtype) {
			windows[window] = subtype;
		}
	}

	/* How much space do we need in the rdf for those windows?
	 */
	sz = 0;
	for (i = 0; i < 256; i++) {
		if (windows[i]) {
			sz += windows[i] / 8 + 3;
		}
	}
	if (sz > 0) {
		/* Format rdf data according RFC3845 Section 2.1.2 (see above)
		 */
		dptr = data = LDNS_XMALLOC(uint8_t, sz);
		if (!data) {
			return LDNS_STATUS_MEM_ERR;
		}
		memset(data, value, sz);
		for (i = 0; i < 256; i++) {
			if (windows[i]) {
				*dptr++ = (uint8_t)i;
				*dptr++ = (uint8_t)(windows[i] / 8 + 1);
				dptr += dptr[-1];
			}
		}
	}
	/* Allocate and return rdf structure for the data
	 */
	*rdf = ldns_rdf_new(LDNS_RDF_TYPE_BITMAP, sz, data);
	if (!*rdf) {
		LDNS_FREE(data);
		return LDNS_STATUS_MEM_ERR;
	}
	return LDNS_STATUS_OK;
}
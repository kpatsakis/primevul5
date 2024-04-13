int ldns_rr_compare_wire(const ldns_buffer *rr1_buf, const ldns_buffer *rr2_buf)
{
        size_t rr1_len, rr2_len, min_len, i, offset;

        rr1_len = ldns_buffer_capacity(rr1_buf);
        rr2_len = ldns_buffer_capacity(rr2_buf);

        /* jump past dname (checked in earlier part)
         * and especially past TTL */
        offset = 0;
        while (offset < rr1_len && *ldns_buffer_at(rr1_buf, offset) != 0) {
          offset += *ldns_buffer_at(rr1_buf, offset) + 1;
        }
        /* jump to rdata section (PAST the rdata length field, otherwise
           rrs with different lengths might be sorted erroneously */
        offset += 11;
	   min_len = (rr1_len < rr2_len) ? rr1_len : rr2_len;
        /* Compare RRs RDATA byte for byte. */
        for(i = offset; i < min_len; i++) {
                if (*ldns_buffer_at(rr1_buf,i) < *ldns_buffer_at(rr2_buf,i)) {
                        return -1;
                } else if (*ldns_buffer_at(rr1_buf,i) > *ldns_buffer_at(rr2_buf,i)) {
                        return +1;
                }
        }

        /* If both RDATAs are the same up to min_len, then the shorter one sorts first. */
        if (rr1_len < rr2_len) {
                return -1;
        } else if (rr1_len > rr2_len) {
                return +1;
	}
        /* The RDATAs are equal. */
        return 0;

}
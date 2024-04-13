struct sctp_chunk *sctp_make_asconf_update_ip(struct sctp_association *asoc,
					      union sctp_addr	      *laddr,
					      struct sockaddr	      *addrs,
					      int		      addrcnt,
					      __be16		      flags)
{
	sctp_addip_param_t	param;
	struct sctp_chunk	*retval;
	union sctp_addr_param	addr_param;
	union sctp_addr		*addr;
	void			*addr_buf;
	struct sctp_af		*af;
	int			paramlen = sizeof(param);
	int			addr_param_len = 0;
	int 			totallen = 0;
	int 			i;

	/* Get total length of all the address parameters. */
	addr_buf = addrs;
	for (i = 0; i < addrcnt; i++) {
		addr = (union sctp_addr *)addr_buf;
		af = sctp_get_af_specific(addr->v4.sin_family);
		addr_param_len = af->to_addr_param(addr, &addr_param);

		totallen += paramlen;
		totallen += addr_param_len;

		addr_buf += af->sockaddr_len;
	}

	/* Create an asconf chunk with the required length. */
	retval = sctp_make_asconf(asoc, laddr, totallen);
	if (!retval)
		return NULL;

	/* Add the address parameters to the asconf chunk. */
	addr_buf = addrs;
	for (i = 0; i < addrcnt; i++) {
		addr = (union sctp_addr *)addr_buf;
		af = sctp_get_af_specific(addr->v4.sin_family);
		addr_param_len = af->to_addr_param(addr, &addr_param);
		param.param_hdr.type = flags;
		param.param_hdr.length = htons(paramlen + addr_param_len);
		param.crr_id = i;

		sctp_addto_chunk(retval, paramlen, &param);
		sctp_addto_chunk(retval, addr_param_len, &addr_param);

		addr_buf += af->sockaddr_len;
	}
	return retval;
}
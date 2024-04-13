ppp_hdlc_if_print(netdissect_options *ndo,
                  const struct pcap_pkthdr *h, register const u_char *p)
{
	register u_int length = h->len;
	register u_int caplen = h->caplen;
	u_int proto;
	u_int hdrlen = 0;

	if (caplen < 2) {
		ND_PRINT((ndo, "[|ppp]"));
		return (caplen);
	}

	switch (p[0]) {

	case PPP_ADDRESS:
		if (caplen < 4) {
			ND_PRINT((ndo, "[|ppp]"));
			return (caplen);
		}

		if (ndo->ndo_eflag)
			ND_PRINT((ndo, "%02x %02x %d ", p[0], p[1], length));
		p += 2;
		length -= 2;
		hdrlen += 2;

		proto = EXTRACT_16BITS(p);
		p += 2;
		length -= 2;
		hdrlen += 2;
		ND_PRINT((ndo, "%s: ", tok2str(ppptype2str, "unknown PPP protocol (0x%04x)", proto)));

		handle_ppp(ndo, proto, p, length);
		break;

	case CHDLC_UNICAST:
	case CHDLC_BCAST:
		return (chdlc_if_print(ndo, h, p));

	default:
		if (caplen < 4) {
			ND_PRINT((ndo, "[|ppp]"));
			return (caplen);
		}

		if (ndo->ndo_eflag)
			ND_PRINT((ndo, "%02x %02x %d ", p[0], p[1], length));
		p += 2;
		hdrlen += 2;

		/*
		 * XXX - NetBSD's "ppp_netbsd_serial_if_print()" treats
		 * the next two octets as an Ethernet type; does that
		 * ever happen?
		 */
		ND_PRINT((ndo, "unknown addr %02x; ctrl %02x", p[0], p[1]));
		break;
	}

	return (hdrlen);
}
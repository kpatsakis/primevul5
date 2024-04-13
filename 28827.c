int dns_get_ip_from_response(struct dns_response_packet *dns_p,
                             struct dns_options *dns_opts, void *currentip,
                             short currentip_sin_family,
                             void **newip, short *newip_sin_family,
                             void *owner)
{
	struct dns_answer_item *record;
	int family_priority;
	int currentip_found;
	unsigned char *newip4, *newip6;
	int currentip_sel;
	int j;
	int score, max_score;
	int allowed_duplicated_ip;

	family_priority   = dns_opts->family_prio;
	allowed_duplicated_ip = dns_opts->accept_duplicate_ip;
	*newip = newip4   = newip6 = NULL;
	currentip_found   = 0;
	*newip_sin_family = AF_UNSPEC;
	max_score         = -1;

	/* Select an IP regarding configuration preference.
	 * Top priority is the preferred network ip version,
	 * second priority is the preferred network.
	 * the last priority is the currently used IP,
	 *
	 * For these three priorities, a score is calculated. The
	 * weight are:
	 *  8 - preferred ip version.
	 *  4 - preferred network.
	 *  2 - if the ip in the record is not affected to any other server in the same backend (duplication)
	 *  1 - current ip.
	 * The result with the biggest score is returned.
	 */

	list_for_each_entry(record, &dns_p->answer_list, list) {
		void *ip;
		unsigned char ip_type;

		if (record->type == DNS_RTYPE_A) {
			ip = &(((struct sockaddr_in *)&record->address)->sin_addr);
			ip_type = AF_INET;
		}
		else if (record->type == DNS_RTYPE_AAAA) {
			ip_type = AF_INET6;
			ip = &(((struct sockaddr_in6 *)&record->address)->sin6_addr);
		}
		else
			continue;
		score = 0;

		/* Check for preferred ip protocol. */
		if (ip_type == family_priority)
			score += 8;

		/* Check for preferred network. */
		for (j = 0; j < dns_opts->pref_net_nb; j++) {

			/* Compare only the same adresses class. */
			if (dns_opts->pref_net[j].family != ip_type)
				continue;

			if ((ip_type == AF_INET &&
			     in_net_ipv4(ip,
			                 &dns_opts->pref_net[j].mask.in4,
			                 &dns_opts->pref_net[j].addr.in4)) ||
			    (ip_type == AF_INET6 &&
			     in_net_ipv6(ip,
			                 &dns_opts->pref_net[j].mask.in6,
			                 &dns_opts->pref_net[j].addr.in6))) {
				score += 4;
				break;
			}
		}

		/* Check if the IP found in the record is already affected to a
		 * member of a group.  If not, the score should be incremented
		 * by 2. */
		if (owner && snr_check_ip_callback(owner, ip, &ip_type)) {
			if (!allowed_duplicated_ip) {
				continue;
			}
		} else {
			score += 2;
		}

		/* Check for current ip matching. */
		if (ip_type == currentip_sin_family &&
		    ((currentip_sin_family == AF_INET &&
		      !memcmp(ip, currentip, 4)) ||
		     (currentip_sin_family == AF_INET6 &&
		      !memcmp(ip, currentip, 16)))) {
			score++;
			currentip_sel = 1;
		}
		else
			currentip_sel = 0;

		/* Keep the address if the score is better than the previous
		 * score. The maximum score is 15, if this value is reached, we
		 * break the parsing. Implicitly, this score is reached the ip
		 * selected is the current ip. */
		if (score > max_score) {
			if (ip_type == AF_INET)
				newip4 = ip;
			else
				newip6 = ip;
			currentip_found = currentip_sel;
			if (score == 15)
				return DNS_UPD_NO;
			max_score = score;
		}
	} /* list for each record entries */

	/* No IP found in the response */
	if (!newip4 && !newip6)
		return DNS_UPD_NO_IP_FOUND;

	/* Case when the caller looks first for an IPv4 address */
	if (family_priority == AF_INET) {
		if (newip4) {
			*newip = newip4;
			*newip_sin_family = AF_INET;
		}
		else if (newip6) {
			*newip = newip6;
			*newip_sin_family = AF_INET6;
		}
		if (!currentip_found)
			goto not_found;
	}
	/* Case when the caller looks first for an IPv6 address */
	else if (family_priority == AF_INET6) {
		if (newip6) {
			*newip = newip6;
			*newip_sin_family = AF_INET6;
		}
		else if (newip4) {
			*newip = newip4;
			*newip_sin_family = AF_INET;
		}
		if (!currentip_found)
			goto not_found;
	}
	/* Case when the caller have no preference (we prefer IPv6) */
	else if (family_priority == AF_UNSPEC) {
		if (newip6) {
			*newip = newip6;
			*newip_sin_family = AF_INET6;
		}
		else if (newip4) {
			*newip = newip4;
			*newip_sin_family = AF_INET;
		}
		if (!currentip_found)
			goto not_found;
	}

	/* No reason why we should change the server's IP address */
	return DNS_UPD_NO;

 not_found:
	list_for_each_entry(record, &dns_p->answer_list, list) {
		/* Move the first record to the end of the list, for internal
		 * round robin */
		LIST_DEL(&record->list);
		LIST_ADDQ(&dns_p->answer_list, &record->list);
		break;
	}
	return DNS_UPD_SRVIP_NOT_FOUND;
}

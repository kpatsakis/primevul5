static int irda_discover_daddr_and_lsap_sel(struct irda_sock *self, char *name)
{
	discinfo_t *discoveries;	/* Copy of the discovery log */
	int	number;			/* Number of nodes in the log */
	int	i;
	int	err = -ENETUNREACH;
	__u32	daddr = DEV_ADDR_ANY;	/* Address we found the service on */
	__u8	dtsap_sel = 0x0;	/* TSAP associated with it */

	IRDA_DEBUG(2, "%s(), name=%s\n", __func__, name);

	/* Ask lmp for the current discovery log
	 * Note : we have to use irlmp_get_discoveries(), as opposed
	 * to play with the cachelog directly, because while we are
	 * making our ias query, le log might change... */
	discoveries = irlmp_get_discoveries(&number, self->mask.word,
					    self->nslots);
	/* Check if the we got some results */
	if (discoveries == NULL)
		return -ENETUNREACH;	/* No nodes discovered */

	/*
	 * Now, check all discovered devices (if any), and connect
	 * client only about the services that the client is
	 * interested in...
	 */
	for(i = 0; i < number; i++) {
		/* Try the address in the log */
		self->daddr = discoveries[i].daddr;
		self->saddr = 0x0;
		IRDA_DEBUG(1, "%s(), trying daddr = %08x\n",
			   __func__, self->daddr);

		/* Query remote LM-IAS for this service */
		err = irda_find_lsap_sel(self, name);
		switch (err) {
		case 0:
			/* We found the requested service */
			if(daddr != DEV_ADDR_ANY) {
				IRDA_DEBUG(1, "%s(), discovered service ''%s'' in two different devices !!!\n",
					   __func__, name);
				self->daddr = DEV_ADDR_ANY;
				kfree(discoveries);
				return(-ENOTUNIQ);
			}
			/* First time we found that one, save it ! */
			daddr = self->daddr;
			dtsap_sel = self->dtsap_sel;
			break;
		case -EADDRNOTAVAIL:
			/* Requested service simply doesn't exist on this node */
			break;
		default:
			/* Something bad did happen :-( */
			IRDA_DEBUG(0, "%s(), unexpected IAS query failure\n", __func__);
			self->daddr = DEV_ADDR_ANY;
			kfree(discoveries);
			return(-EHOSTUNREACH);
			break;
		}
	}
	/* Cleanup our copy of the discovery log */
	kfree(discoveries);

	/* Check out what we found */
	if(daddr == DEV_ADDR_ANY) {
		IRDA_DEBUG(1, "%s(), cannot discover service ''%s'' in any device !!!\n",
			   __func__, name);
		self->daddr = DEV_ADDR_ANY;
		return(-EADDRNOTAVAIL);
	}

	/* Revert back to discovered device & service */
	self->daddr = daddr;
	self->saddr = 0x0;
	self->dtsap_sel = dtsap_sel;

	IRDA_DEBUG(1, "%s(), discovered requested service ''%s'' at address %08x\n",
		   __func__, name, self->daddr);

	return 0;
}
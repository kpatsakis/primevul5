static int irda_find_lsap_sel(struct irda_sock *self, char *name)
{
	IRDA_DEBUG(2, "%s(%p, %s)\n", __func__, self, name);

	if (self->iriap) {
		IRDA_WARNING("%s(): busy with a previous query\n",
			     __func__);
		return -EBUSY;
	}

	self->iriap = iriap_open(LSAP_ANY, IAS_CLIENT, self,
				 irda_getvalue_confirm);
	if(self->iriap == NULL)
		return -ENOMEM;

	/* Treat unexpected wakeup as disconnect */
	self->errno = -EHOSTUNREACH;

	/* Query remote LM-IAS */
	iriap_getvaluebyclass_request(self->iriap, self->saddr, self->daddr,
				      name, "IrDA:TinyTP:LsapSel");

	/* Wait for answer, if not yet finished (or failed) */
	if (wait_event_interruptible(self->query_wait, (self->iriap==NULL)))
		/* Treat signals as disconnect */
		return -EHOSTUNREACH;

	/* Check what happened */
	if (self->errno)
	{
		/* Requested object/attribute doesn't exist */
		if((self->errno == IAS_CLASS_UNKNOWN) ||
		   (self->errno == IAS_ATTRIB_UNKNOWN))
			return (-EADDRNOTAVAIL);
		else
			return (-EHOSTUNREACH);
	}

	/* Get the remote TSAP selector */
	switch (self->ias_result->type) {
	case IAS_INTEGER:
		IRDA_DEBUG(4, "%s() int=%d\n",
			   __func__, self->ias_result->t.integer);

		if (self->ias_result->t.integer != -1)
			self->dtsap_sel = self->ias_result->t.integer;
		else
			self->dtsap_sel = 0;
		break;
	default:
		self->dtsap_sel = 0;
		IRDA_DEBUG(0, "%s(), bad type!\n", __func__);
		break;
	}
	if (self->ias_result)
		irias_delete_value(self->ias_result);

	if (self->dtsap_sel)
		return 0;

	return -EADDRNOTAVAIL;
}
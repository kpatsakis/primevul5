static void _isdn_setup(struct net_device *dev)
{
	isdn_net_local *lp = dev->priv;

	dev->flags = IFF_NOARP | IFF_POINTOPOINT;
	lp->p_encap = ISDN_NET_ENCAP_RAWIP;
	lp->magic = ISDN_NET_MAGIC;
	lp->last = lp;
	lp->next = lp;
	lp->isdn_device = -1;
	lp->isdn_channel = -1;
	lp->pre_device = -1;
	lp->pre_channel = -1;
	lp->exclusive = -1;
	lp->ppp_slot = -1;
	lp->pppbind = -1;
	skb_queue_head_init(&lp->super_tx_queue);
	lp->l2_proto = ISDN_PROTO_L2_X75I;
	lp->l3_proto = ISDN_PROTO_L3_TRANS;
	lp->triggercps = 6000;
	lp->slavedelay = 10 * HZ;
	lp->hupflags = ISDN_INHUP;	/* Do hangup even on incoming calls */
	lp->onhtime = 10;	/* Default hangup-time for saving costs */
	lp->dialmax = 1;
	/* Hangup before Callback, manual dial */
	lp->flags = ISDN_NET_CBHUP | ISDN_NET_DM_MANUAL;
	lp->cbdelay = 25;	/* Wait 5 secs before Callback */
	lp->dialtimeout = -1;  /* Infinite Dial-Timeout */
	lp->dialwait = 5 * HZ; /* Wait 5 sec. after failed dial */
	lp->dialstarted = 0;   /* Jiffies of last dial-start */
	lp->dialwait_timer = 0;  /* Jiffies of earliest next dial-start */
}
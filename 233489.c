isdn_net_xmit(struct net_device *ndev, struct sk_buff *skb)
{
	isdn_net_dev *nd;
	isdn_net_local *slp;
	isdn_net_local *lp = (isdn_net_local *) ndev->priv;
	int retv = 0;

	if (((isdn_net_local *) (ndev->priv))->master) {
		printk("isdn BUG at %s:%d!\n", __FILE__, __LINE__);
		dev_kfree_skb(skb);
		return 0;
	}

	/* For the other encaps the header has already been built */
#ifdef CONFIG_ISDN_PPP
	if (lp->p_encap == ISDN_NET_ENCAP_SYNCPPP) {
		return isdn_ppp_xmit(skb, ndev);
	}
#endif
	nd = ((isdn_net_local *) ndev->priv)->netdev;
	lp = isdn_net_get_locked_lp(nd);
	if (!lp) {
		printk(KERN_WARNING "%s: all channels busy - requeuing!\n", ndev->name);
		return 1;
	}
	/* we have our lp locked from now on */

	/* Reset hangup-timeout */
	lp->huptimer = 0; // FIXME?
	isdn_net_writebuf_skb(lp, skb);
	spin_unlock_bh(&lp->xmit_lock);

	/* the following stuff is here for backwards compatibility.
	 * in future, start-up and hangup of slaves (based on current load)
	 * should move to userspace and get based on an overall cps
	 * calculation
	 */
	if (lp->cps > lp->triggercps) {
		if (lp->slave) {
			if (!lp->sqfull) {
				/* First time overload: set timestamp only */
				lp->sqfull = 1;
				lp->sqfull_stamp = jiffies;
			} else {
				/* subsequent overload: if slavedelay exceeded, start dialing */
				if (time_after(jiffies, lp->sqfull_stamp + lp->slavedelay)) {
					slp = lp->slave->priv;
					if (!(slp->flags & ISDN_NET_CONNECTED)) {
						isdn_net_force_dial_lp((isdn_net_local *) lp->slave->priv);
					}
				}
			}
		}
	} else {
		if (lp->sqfull && time_after(jiffies, lp->sqfull_stamp + lp->slavedelay + (10 * HZ))) {
			lp->sqfull = 0;
		}
		/* this is a hack to allow auto-hangup for slaves on moderate loads */
		nd->queue = nd->local;
	}

	return retv;

}
void tipc_crypto_timeout(struct tipc_crypto *rx)
{
	struct tipc_net *tn = tipc_net(rx->net);
	struct tipc_crypto *tx = tn->crypto_tx;
	struct tipc_key key;
	int cmd;

	/* TX pending: taking all users & stable -> active */
	spin_lock(&tx->lock);
	key = tx->key;
	if (key.active && tipc_aead_users(tx->aead[key.active]) > 0)
		goto s1;
	if (!key.pending || tipc_aead_users(tx->aead[key.pending]) <= 0)
		goto s1;
	if (time_before(jiffies, tx->timer1 + TIPC_TX_LASTING_TIME))
		goto s1;

	tipc_crypto_key_set_state(tx, key.passive, key.pending, 0);
	if (key.active)
		tipc_crypto_key_detach(tx->aead[key.active], &tx->lock);
	this_cpu_inc(tx->stats->stat[STAT_SWITCHES]);
	pr_info("%s: key[%d] is activated\n", tx->name, key.pending);

s1:
	spin_unlock(&tx->lock);

	/* RX pending: having user -> active */
	spin_lock(&rx->lock);
	key = rx->key;
	if (!key.pending || tipc_aead_users(rx->aead[key.pending]) <= 0)
		goto s2;

	if (key.active)
		key.passive = key.active;
	key.active = key.pending;
	rx->timer2 = jiffies;
	tipc_crypto_key_set_state(rx, key.passive, key.active, 0);
	this_cpu_inc(rx->stats->stat[STAT_SWITCHES]);
	pr_info("%s: key[%d] is activated\n", rx->name, key.pending);
	goto s5;

s2:
	/* RX pending: not working -> remove */
	if (!key.pending || tipc_aead_users(rx->aead[key.pending]) > -10)
		goto s3;

	tipc_crypto_key_set_state(rx, key.passive, key.active, 0);
	tipc_crypto_key_detach(rx->aead[key.pending], &rx->lock);
	pr_debug("%s: key[%d] is removed\n", rx->name, key.pending);
	goto s5;

s3:
	/* RX active: timed out or no user -> pending */
	if (!key.active)
		goto s4;
	if (time_before(jiffies, rx->timer1 + TIPC_RX_ACTIVE_LIM) &&
	    tipc_aead_users(rx->aead[key.active]) > 0)
		goto s4;

	if (key.pending)
		key.passive = key.active;
	else
		key.pending = key.active;
	rx->timer2 = jiffies;
	tipc_crypto_key_set_state(rx, key.passive, 0, key.pending);
	tipc_aead_users_set(rx->aead[key.pending], 0);
	pr_debug("%s: key[%d] is deactivated\n", rx->name, key.active);
	goto s5;

s4:
	/* RX passive: outdated or not working -> free */
	if (!key.passive)
		goto s5;
	if (time_before(jiffies, rx->timer2 + TIPC_RX_PASSIVE_LIM) &&
	    tipc_aead_users(rx->aead[key.passive]) > -10)
		goto s5;

	tipc_crypto_key_set_state(rx, 0, key.active, key.pending);
	tipc_crypto_key_detach(rx->aead[key.passive], &rx->lock);
	pr_debug("%s: key[%d] is freed\n", rx->name, key.passive);

s5:
	spin_unlock(&rx->lock);

	/* Relax it here, the flag will be set again if it really is, but only
	 * when we are not in grace period for safety!
	 */
	if (time_after(jiffies, tx->timer2 + TIPC_TX_GRACE_PERIOD))
		tx->legacy_user = 0;

	/* Limit max_tfms & do debug commands if needed */
	if (likely(sysctl_tipc_max_tfms <= TIPC_MAX_TFMS_LIM))
		return;

	cmd = sysctl_tipc_max_tfms;
	sysctl_tipc_max_tfms = TIPC_MAX_TFMS_DEF;
	tipc_crypto_do_cmd(rx->net, cmd);
}
/*
 * Returns the matching session to a given sid
 */
static struct iscsi_cls_session *iscsi_session_lookup(uint32_t sid)
{
	unsigned long flags;
	struct iscsi_cls_session *sess;

	spin_lock_irqsave(&sesslock, flags);
	list_for_each_entry(sess, &sesslist, sess_list) {
		if (sess->sid == sid) {
			spin_unlock_irqrestore(&sesslock, flags);
			return sess;
		}
	}
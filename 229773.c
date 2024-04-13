static void nfs41_release_slot(struct nfs4_slot *slot)
{
	struct nfs4_session *session;
	struct nfs4_slot_table *tbl;
	bool send_new_highest_used_slotid = false;

	if (!slot)
		return;
	tbl = slot->table;
	session = tbl->session;

	/* Bump the slot sequence number */
	if (slot->seq_done)
		slot->seq_nr++;
	slot->seq_done = 0;

	spin_lock(&tbl->slot_tbl_lock);
	/* Be nice to the server: try to ensure that the last transmitted
	 * value for highest_user_slotid <= target_highest_slotid
	 */
	if (tbl->highest_used_slotid > tbl->target_highest_slotid)
		send_new_highest_used_slotid = true;

	if (nfs41_wake_and_assign_slot(tbl, slot)) {
		send_new_highest_used_slotid = false;
		goto out_unlock;
	}
	nfs4_free_slot(tbl, slot);

	if (tbl->highest_used_slotid != NFS4_NO_SLOT)
		send_new_highest_used_slotid = false;
out_unlock:
	spin_unlock(&tbl->slot_tbl_lock);
	if (send_new_highest_used_slotid)
		nfs41_notify_server(session->clp);
	if (waitqueue_active(&tbl->slot_waitq))
		wake_up_all(&tbl->slot_waitq);
}
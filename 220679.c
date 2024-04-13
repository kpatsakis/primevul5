static void packet_rcv_try_clear_pressure(struct packet_sock *po)
{
	if (READ_ONCE(po->pressure) &&
	    __packet_rcv_has_room(po, NULL) == ROOM_NORMAL)
		WRITE_ONCE(po->pressure,  0);
}
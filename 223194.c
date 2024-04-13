static void ping_clients(int tile_cnt) {
	static time_t last_send = 0;
	time_t now = time(NULL);

	if (rfbMaxClientWait < 20000) {
		rfbMaxClientWait = 20000;
		rfbLog("reset rfbMaxClientWait to %d msec.\n",
		    rfbMaxClientWait);
	}
	if (tile_cnt > 0) {
		last_send = now;
	} else if (tile_cnt < 0) {
		/* negative tile_cnt is -ping case */
		if (now >= last_send - tile_cnt) {
			mark_rect_as_modified(0, 0, 1, 1, 1);
			last_send = now;
		}
	} else if (now - last_send > 5) {
		/* Send small heartbeat to client */
		mark_rect_as_modified(0, 0, 1, 1, 1);
		last_send = now;
	}
}
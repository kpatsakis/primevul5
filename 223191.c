static void nap_check(int tile_cnt) {
	time_t now;

	nap_diff_count += tile_cnt;

	if (! take_naps) {
		return;
	}

	now = time(NULL);

	if (screen_blank > 0) {
		int dt_ev, dt_fbu;
		static int ms = 0;
		if (ms == 0) {
			ms = 2000;
			if (getenv("X11VNC_SB_FACTOR")) {
				ms = ms * atof(getenv("X11VNC_SB_FACTOR"));
			}
			if (ms <= 0) {
				ms = 2000;
			}
		}

		/* if no activity, pause here for a second or so. */
		dt_ev  = (int) (now - last_event);
		dt_fbu = (int) (now - last_fb_bytes_sent);
		if (dt_fbu > screen_blank) {
			/* sleep longer for no fb requests */
			if (debug_tiles > 1) {
				fprintf(stderr, "screen blank sleep1: %d ms / 16, load: %s\n", 2 * ms, get_load());
			}
			nap_sleep(2 * ms, 16);
			return;
		}
		if (dt_ev > screen_blank) {
			if (debug_tiles > 1) {
				fprintf(stderr, "screen blank sleep2: %d ms / 8, load: %s\n", ms, get_load());
			}
			nap_sleep(ms, 8);
			return;
		}
	}
	if (naptile && nap_ok && tile_cnt < naptile) {
		int ms = napfac * waitms;
		ms = ms > napmax ? napmax : ms;
		if (now - last_input <= 3) {
			nap_ok = 0;
		} else if (now - last_local_input <= 3) {
			nap_ok = 0;
		} else {
			if (debug_tiles > 1) {
				fprintf(stderr, "nap_check sleep: %d ms / 1, load: %s\n", ms, get_load());
			}
			nap_sleep(ms, 1);
		}
	}
}
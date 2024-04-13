void shm_clean(XShmSegmentInfo *shm, XImage *xim) {
	int db = 0;

	if (db) fprintf(stderr, "shm_clean: called:  %p\n", (void *)xim);
	X_LOCK;
#if HAVE_XSHM
	if (shm != NULL && shm->shmid != -1 && dpy) {
		if (db) fprintf(stderr, "shm_clean: XShmDetach_wr\n");
		XShmDetach_wr(dpy, shm);
	}
#endif
	if (xim != NULL) {
		if (! raw_fb_back_to_X) {	/* raw_fb hack */
			if (xim->bitmap_unit != -1) {
				if (db) fprintf(stderr, "shm_clean: XDestroyImage  %p\n", (void *)xim);
				XDestroyImage(xim);
			} else {
				if (xim->data) {
					if (db) fprintf(stderr, "shm_clean: free xim->data  %p %p\n", (void *)xim, (void *)(xim->data));
					free(xim->data);
					xim->data = NULL;
				}
			}
		}
		xim = NULL;
	}
	X_UNLOCK;

	shm_delete(shm);
}
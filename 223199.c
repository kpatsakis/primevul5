void shm_delete(XShmSegmentInfo *shm) {
#if HAVE_XSHM
	if (getenv("X11VNC_SHM_DEBUG")) fprintf(stderr, "shm_delete:    %p\n", (void *) shm);
	if (shm != NULL && shm->shmaddr != (char *) -1) {
		shmdt(shm->shmaddr);
	}
	if (shm != NULL && shm->shmid != -1) {
		shmctl(shm->shmid, IPC_RMID, 0);
	}
	if (shm != NULL) {
		shm->shmaddr = (char *) -1;
		shm->shmid = -1;
	}
#else
	if (!shm) {}
#endif
}
processWorkItem(epolld_t *epd)
{
	int continue_polling = 1;

	switch(epd->typ) {
	case epolld_lstn:
		/* listener never stops polling (except server shutdown) */
		lstnActivity((ptcplstn_t *) epd->ptr);
		break;
	case epolld_sess:
		sessActivity((ptcpsess_t *) epd->ptr, &continue_polling);
		break;
	default:
		errmsg.LogError(0, RS_RET_INTERNAL_ERROR,
						"error: invalid epolld_type_t %d after epoll", epd->typ);
		break;
	}
	if (continue_polling == 1) {
		epoll_ctl(epollfd, EPOLL_CTL_MOD, epd->sock, &(epd->ev));
	}
}
free_zmbuf(struct vhost_virtqueue *vq)
{
	struct zcopy_mbuf *next = NULL;
	struct zcopy_mbuf *zmbuf;

	for (zmbuf = TAILQ_FIRST(&vq->zmbuf_list);
	     zmbuf != NULL; zmbuf = next) {
		next = TAILQ_NEXT(zmbuf, next);

		uint16_t last_used_idx = vq->last_used_idx;

		if (mbuf_is_consumed(zmbuf->mbuf)) {
			uint16_t flags;
			flags = vq->desc_packed[last_used_idx].flags;
			if (vq->used_wrap_counter) {
				flags |= VRING_DESC_F_USED;
				flags |= VRING_DESC_F_AVAIL;
			} else {
				flags &= ~VRING_DESC_F_USED;
				flags &= ~VRING_DESC_F_AVAIL;
			}

			vq->desc_packed[last_used_idx].id = zmbuf->desc_idx;
			vq->desc_packed[last_used_idx].len = 0;

			rte_smp_wmb();
			vq->desc_packed[last_used_idx].flags = flags;

			vq_inc_last_used_packed(vq, zmbuf->desc_count);

			TAILQ_REMOVE(&vq->zmbuf_list, zmbuf, next);
			restore_mbuf(zmbuf->mbuf);
			rte_pktmbuf_free(zmbuf->mbuf);
			put_zmbuf(zmbuf);
			vq->nr_zmbuf -= 1;
		}
	}
}
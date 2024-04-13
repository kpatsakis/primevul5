do_data_copy_enqueue(struct virtio_net *dev, struct vhost_virtqueue *vq)
{
	struct batch_copy_elem *elem = vq->batch_copy_elems;
	uint16_t count = vq->batch_copy_nb_elems;
	int i;

	for (i = 0; i < count; i++) {
		rte_memcpy(elem[i].dst, elem[i].src, elem[i].len);
		vhost_log_cache_write_iova(dev, vq, elem[i].log_addr,
					   elem[i].len);
		PRINT_PACKET(dev, (uintptr_t)elem[i].dst, elem[i].len, 0);
	}

	vq->batch_copy_nb_elems = 0;
}
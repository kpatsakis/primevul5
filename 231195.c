static int vhost_net_buf_produce(struct vhost_net_virtqueue *nvq)
{
	struct vhost_net_buf *rxq = &nvq->rxq;

	rxq->head = 0;
	rxq->tail = ptr_ring_consume_batched(nvq->rx_ring, rxq->queue,
					      VHOST_NET_BATCH);
	return rxq->tail;
}
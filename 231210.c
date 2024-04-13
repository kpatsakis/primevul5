static void vhost_net_busy_poll_try_queue(struct vhost_net *net,
					  struct vhost_virtqueue *vq)
{
	if (!vhost_vq_avail_empty(&net->dev, vq)) {
		vhost_poll_queue(&vq->poll);
	} else if (unlikely(vhost_enable_notify(&net->dev, vq))) {
		vhost_disable_notify(&net->dev, vq);
		vhost_poll_queue(&vq->poll);
	}
}
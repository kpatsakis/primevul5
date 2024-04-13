static void vhost_net_busy_poll(struct vhost_net *net,
				struct vhost_virtqueue *rvq,
				struct vhost_virtqueue *tvq,
				bool *busyloop_intr,
				bool poll_rx)
{
	unsigned long busyloop_timeout;
	unsigned long endtime;
	struct socket *sock;
	struct vhost_virtqueue *vq = poll_rx ? tvq : rvq;

	/* Try to hold the vq mutex of the paired virtqueue. We can't
	 * use mutex_lock() here since we could not guarantee a
	 * consistenet lock ordering.
	 */
	if (!mutex_trylock(&vq->mutex))
		return;

	vhost_disable_notify(&net->dev, vq);
	sock = rvq->private_data;

	busyloop_timeout = poll_rx ? rvq->busyloop_timeout:
				     tvq->busyloop_timeout;

	preempt_disable();
	endtime = busy_clock() + busyloop_timeout;

	while (vhost_can_busy_poll(endtime)) {
		if (vhost_has_work(&net->dev)) {
			*busyloop_intr = true;
			break;
		}

		if ((sock_has_rx_data(sock) &&
		     !vhost_vq_avail_empty(&net->dev, rvq)) ||
		    !vhost_vq_avail_empty(&net->dev, tvq))
			break;

		cpu_relax();
	}

	preempt_enable();

	if (poll_rx || sock_has_rx_data(sock))
		vhost_net_busy_poll_try_queue(net, vq);
	else if (!poll_rx) /* On tx here, sock has no rx data. */
		vhost_enable_notify(&net->dev, rvq);

	mutex_unlock(&vq->mutex);
}
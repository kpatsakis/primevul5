static int vhost_net_set_backend_features(struct vhost_net *n, u64 features)
{
	int i;

	mutex_lock(&n->dev.mutex);
	for (i = 0; i < VHOST_NET_VQ_MAX; ++i) {
		mutex_lock(&n->vqs[i].vq.mutex);
		n->vqs[i].vq.acked_backend_features = features;
		mutex_unlock(&n->vqs[i].vq.mutex);
	}
	mutex_unlock(&n->dev.mutex);

	return 0;
}
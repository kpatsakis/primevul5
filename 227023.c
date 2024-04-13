virtio_net_is_inorder(struct virtio_net *dev)
{
	return dev->features & (1ULL << VIRTIO_F_IN_ORDER);
}
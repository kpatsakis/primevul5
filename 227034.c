virtio_dev_extbuf_free(void *addr __rte_unused, void *opaque)
{
	rte_free(opaque);
}
update_shadow_used_ring_split(struct vhost_virtqueue *vq,
			 uint16_t desc_idx, uint32_t len)
{
	uint16_t i = vq->shadow_used_idx++;

	vq->shadow_used_split[i].id  = desc_idx;
	vq->shadow_used_split[i].len = len;
}
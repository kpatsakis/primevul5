static void vhost_vdpa_reset(struct vhost_vdpa *v)
{
	struct vdpa_device *vdpa = v->vdpa;

	vdpa_reset(vdpa);
	v->in_batch = 0;
}
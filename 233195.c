static void vhost_vdpa_set_iova_range(struct vhost_vdpa *v)
{
	struct vdpa_iova_range *range = &v->range;
	struct iommu_domain_geometry geo;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;

	if (ops->get_iova_range) {
		*range = ops->get_iova_range(vdpa);
	} else if (v->domain &&
		   !iommu_domain_get_attr(v->domain,
		   DOMAIN_ATTR_GEOMETRY, &geo) &&
		   geo.force_aperture) {
		range->first = geo.aperture_start;
		range->last = geo.aperture_end;
	} else {
		range->first = 0;
		range->last = ULLONG_MAX;
	}
}
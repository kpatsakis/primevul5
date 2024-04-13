static struct mutex *v4l2_ioctl_get_lock(struct video_device *vdev,
					 struct v4l2_fh *vfh, unsigned int cmd,
					 void *arg)
{
	if (_IOC_NR(cmd) >= V4L2_IOCTLS)
		return vdev->lock;
	if (vfh && vfh->m2m_ctx &&
	    (v4l2_ioctls[_IOC_NR(cmd)].flags & INFO_FL_QUEUE)) {
		if (vfh->m2m_ctx->q_lock)
			return vfh->m2m_ctx->q_lock;
	}
	if (vdev->queue && vdev->queue->lock &&
			(v4l2_ioctls[_IOC_NR(cmd)].flags & INFO_FL_QUEUE))
		return vdev->queue->lock;
	return vdev->lock;
}
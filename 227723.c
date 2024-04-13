static struct uvc_video_chain *uvc_alloc_chain(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;

	chain = kzalloc(sizeof(*chain), GFP_KERNEL);
	if (chain == NULL)
		return NULL;

	INIT_LIST_HEAD(&chain->entities);
	mutex_init(&chain->ctrl_mutex);
	chain->dev = dev;
	v4l2_prio_init(&chain->prio);

	return chain;
}
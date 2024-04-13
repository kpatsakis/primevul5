static void uvc_delete(struct kref *kref)
{
	struct uvc_device *dev = container_of(kref, struct uvc_device, ref);
	struct list_head *p, *n;

	uvc_status_cleanup(dev);
	uvc_ctrl_cleanup_device(dev);

	usb_put_intf(dev->intf);
	usb_put_dev(dev->udev);

#ifdef CONFIG_MEDIA_CONTROLLER
	media_device_cleanup(&dev->mdev);
#endif

	list_for_each_safe(p, n, &dev->chains) {
		struct uvc_video_chain *chain;
		chain = list_entry(p, struct uvc_video_chain, list);
		kfree(chain);
	}

	list_for_each_safe(p, n, &dev->entities) {
		struct uvc_entity *entity;
		entity = list_entry(p, struct uvc_entity, list);
#ifdef CONFIG_MEDIA_CONTROLLER
		uvc_mc_cleanup_entity(entity);
#endif
		kfree(entity);
	}

	list_for_each_safe(p, n, &dev->streams) {
		struct uvc_streaming *streaming;
		streaming = list_entry(p, struct uvc_streaming, list);
		usb_driver_release_interface(&uvc_driver.driver,
			streaming->intf);
		uvc_stream_delete(streaming);
	}

	kfree(dev);
}
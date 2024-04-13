static void cdev_default_release(struct kobject *kobj)
{
	struct cdev *p = container_of(kobj, struct cdev, kobj);
	struct kobject *parent = kobj->parent;

	cdev_purge(p);
	kobject_put(parent);
}
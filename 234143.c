void add_disk_randomness(struct gendisk *disk)
{
	if (!disk || !disk->random)
		return;
	/* first major is 1, so we get >= 0x200 here */
	DEBUG_ENT("disk event %d:%d\n",
		  MAJOR(disk_devt(disk)), MINOR(disk_devt(disk)));

	add_timer_randomness(disk->random, 0x100 + disk_devt(disk));
}
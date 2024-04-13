static void __exit input_exit(void)
{
	input_proc_exit();
	unregister_chrdev_region(MKDEV(INPUT_MAJOR, 0),
				 INPUT_MAX_CHAR_DEVICES);
	class_unregister(&input_class);
}
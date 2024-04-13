static void __exit netif_exit(void)
{
	xenbus_unregister_driver(&netfront_driver);
}
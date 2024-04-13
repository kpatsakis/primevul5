usage(void)
{
	printf("usage:  unbound [options]\n");
	printf("	start unbound daemon DNS resolver.\n");
	printf("-h	this help.\n");
	printf("-c file	config file to read instead of %s\n", CONFIGFILE);
	printf("	file format is described in unbound.conf(5).\n");
	printf("-d	do not fork into the background.\n");
	printf("-p	do not create a pidfile.\n");
	printf("-v	verbose (more times to increase verbosity).\n");
	printf("-V	show version number and build options.\n");
#ifdef UB_ON_WINDOWS
	printf("-w opt	windows option: \n");
	printf("   	install, remove - manage the services entry\n");
	printf("   	service - used to start from services control panel\n");
#endif
	printf("\nVersion %s\n", PACKAGE_VERSION);
	printf("BSD licensed, see LICENSE in source package for details.\n");
	printf("Report bugs to %s\n", PACKAGE_BUGREPORT);
}
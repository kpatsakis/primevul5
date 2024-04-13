checkversion(const char *ver)
{
	int major, minor = 0;

	if (sscanf(ver, "%d.%d", &major, &minor) < 1)
		fatal("invalid ninja_required_version");
	if (major > ninjamajor || (major == ninjamajor && minor > ninjaminor))
		fatal("ninja_required_version %s is newer than %d.%d", ver, ninjamajor, ninjaminor);
}
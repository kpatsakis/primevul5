void usage()
{
	printf("Converting from BIND .private-key format to PKCS#8 key file format.\n");
	printf("Usage: softhsm2-keyconv [OPTIONS]\n");
	printf("Options:\n");
	printf("  -h                  Shows this help screen.\n");
	printf("  --help              Shows this help screen.\n");
	printf("  --in <path>         The path to the input file.\n");
	printf("  --out <path>        The path to the output file.\n");
	printf("  --pin <PIN>         To encrypt PKCS#8 file. Optional.\n");
	printf("  -v                  Show version info.\n");
	printf("  --version           Show version info.\n");
}
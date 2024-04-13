int main(int argc, char* argv[])
{
	int option_index = 0;
	int opt, result;

	char* in_path = NULL;
	char* out_path = NULL;
	char* file_pin = NULL;

	if (argc == 1)
	{
		usage();
		exit(0);
	}

	while ((opt = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1)
	{
		switch (opt)
		{
			case OPT_IN:
				in_path = optarg;
				break;
			case OPT_OUT:
				out_path = optarg;
				break;
			case OPT_PIN:
				file_pin = optarg;
				break;
			case OPT_VERSION:
			case 'v':
				printf("%s\n", PACKAGE_VERSION);
				exit(0);
				break;
			case OPT_HELP:
			case 'h':
			default:
				usage();
				exit(0);
				break;
		}
	}

	// We should convert to PKCS#8
	result = to_pkcs8(in_path, out_path, file_pin);

	return result;
}
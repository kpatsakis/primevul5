int get_network_line(char *packet, char *network_line)
{
	int i=0;

	memset(network_line, 0, 8192);

	while ( packet[i] != '\0' && packet[i] != '\n' ) {

		network_line[i]=packet[i];
		i+=1;
	}

	if ( packet[i] == '\0' ) {
		return 0;
	}

	if ( i == 0 ) {
		network_line[i]='\0';
	}

	network_line[i+1]='\0';

	return 1;
}
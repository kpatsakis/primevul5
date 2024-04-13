int read_config(const char *config_file) {
FILE *fp;
char *buffer;
char *token, *value;

  if (config_file==NULL) return -1;

  if( (fp = fopen( config_file, "r" ) ) == NULL ) {
     return -1;
  }

  buffer = (char*)malloc( sizeof(char) * 4096 );

  while( fgets( buffer, 4096, fp ) ) {
    value = trim_whitespace( buffer );
    token = getToken( &value, "=" );
    if( token == NULL )  /* ignore this line if there isn't a token/value pair */
        continue;
    token = trim_whitespace( token );

    if( strcasecmp( token, "TIMEOUT" ) == 0 ) {
        if (value && atol(value)>0) { delay=atol(value); }
#ifdef PROC_NET_DEV
    } else if( strcasecmp( token, "PROCFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_FILE,value);
#endif
#ifdef PROC_DISKSTATS
    } else if( strcasecmp( token, "DISKSTATSFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_DISKSTATS_FILE,value);		  
    } else if( strcasecmp( token, "PARTITIONSFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_PARTITIONS_FILE,value);		  
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    } else if( strcasecmp( token, "NETSTAT" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(NETSTAT_FILE,value);
#endif
#endif
    } else if( strcasecmp( token, "INPUT" ) == 0 ) {
        if (value) input_method=str2in_method(value);
	 } else if( strcasecmp( token, "ANSIOUT" ) == 0 ) {
		 if (value) ansi_output=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "DYNAMIC" ) == 0 ) {
        if (value) dynamic=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "UNIT" ) == 0 ) {
        if (value) output_unit=str2output_unit(value);
#if EXTENDED_STATS
    } else if( strcasecmp( token, "TYPE" ) == 0 ) {
        if (value) output_type=str2output_type(value);
    } else if( strcasecmp( token, "AVGLENGTH" ) == 0 ) {
        if (value) avg_length=atoi(value)*1000;
#endif        
    } else if( strcasecmp( token, "ALLIF" ) == 0 ) {
        if (value) show_all_if=value[0];
    } else if( strcasecmp( token, "INTERFACES" ) == 0 ) {
        if (value) iface_list=(char *)strdup(value);
    } else if( strcasecmp( token, "OUTPUT" ) == 0 ) {
        if (value) output_method=str2out_method(value);
#ifdef CSV
    } else if( strcasecmp( token, "CSVCHAR" ) == 0 ) {
        if (value) csv_char=value[0];
#endif
#if CSV || HTML
    } else if( strcasecmp( token, "OUTFILE" ) == 0 ) {
        if (value) { 
            if (out_file) fclose(out_file);
            out_file=fopen(value,"a"); 
            if (!out_file) deinit(1, "failed to open outfile\n");
            if (out_file_path) free(out_file_path);
            out_file_path=(char *)strdup(value);
        }
#endif
    } else if( strcasecmp( token, "COUNT" ) == 0 ) {
        if (value) output_count=atol(value);
    } else if( strcasecmp( token, "DAEMON" ) == 0 ) {
        if (value) daemonize=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "SUMHIDDEN" ) == 0 ) {
        if (value) sumhidden=value[0]=='0' ? 0 : 1;
#if IOSERVICE_IN
	 } else if( strcasecmp( token, "LONGDISKNAMES" ) == 0) {
		 if (value) long_darwin_disk_names=value[0]=='0' ? 0 : 1;
#endif
#ifdef HTML
    } else if( strcasecmp( token, "HTMLREFRESH" ) == 0 ) {
        if (value && atol(value)>0) { html_refresh=atol(value); }
    } else if( strcasecmp( token, "HTMLHEADER" ) == 0 ) {
        if (value) html_header=value[0]=='0' ? 0 : 1;
#endif
    }
  }
  free(buffer);
  fclose(fp);

 return 0;
}
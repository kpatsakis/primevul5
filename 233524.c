static inline void dccp_mib_exit(void)
{
	snmp_mib_free((void**)dccp_statistics);
}
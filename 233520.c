static inline int dccp_mib_init(void)
{
	return snmp_mib_init((void**)dccp_statistics, sizeof(struct dccp_mib));
}
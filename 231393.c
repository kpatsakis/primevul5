unsigned long long ns2usecs(u64 nsec)
{
	nsec += 500;
	do_div(nsec, 1000);
	return nsec;
}
void ntlm_current_time(BYTE* timestamp)
{
	FILETIME filetime;
	ULARGE_INTEGER time64;
	GetSystemTimeAsFileTime(&filetime);
	time64.u.LowPart = filetime.dwLowDateTime;
	time64.u.HighPart = filetime.dwHighDateTime;
	CopyMemory(timestamp, &(time64.QuadPart), 8);
}
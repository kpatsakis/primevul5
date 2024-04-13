void ntlm_generate_timestamp(NTLM_CONTEXT* context)
{
	if (memcmp(context->ChallengeTimestamp, NTLM_NULL_BUFFER, 8) != 0)
		CopyMemory(context->Timestamp, context->ChallengeTimestamp, 8);
	else
		ntlm_current_time(context->Timestamp);
}
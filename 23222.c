char * __init efi_md_typeattr_format(char *buf, size_t size,
				     const efi_memory_desc_t *md)
{
	char *pos;
	int type_len;
	u64 attr;

	pos = buf;
	if (md->type >= ARRAY_SIZE(memory_type_name))
		type_len = snprintf(pos, size, "[type=%u", md->type);
	else
		type_len = snprintf(pos, size, "[%-*s",
				    (int)(sizeof(memory_type_name[0]) - 1),
				    memory_type_name[md->type]);
	if (type_len >= size)
		return buf;

	pos += type_len;
	size -= type_len;

	attr = md->attribute;
	if (attr & ~(EFI_MEMORY_UC | EFI_MEMORY_WC | EFI_MEMORY_WT |
		     EFI_MEMORY_WB | EFI_MEMORY_UCE | EFI_MEMORY_RO |
		     EFI_MEMORY_WP | EFI_MEMORY_RP | EFI_MEMORY_XP |
		     EFI_MEMORY_NV |
		     EFI_MEMORY_RUNTIME | EFI_MEMORY_MORE_RELIABLE))
		snprintf(pos, size, "|attr=0x%016llx]",
			 (unsigned long long)attr);
	else
		snprintf(pos, size,
			 "|%3s|%2s|%2s|%2s|%2s|%2s|%2s|%3s|%2s|%2s|%2s|%2s]",
			 attr & EFI_MEMORY_RUNTIME ? "RUN" : "",
			 attr & EFI_MEMORY_MORE_RELIABLE ? "MR" : "",
			 attr & EFI_MEMORY_NV      ? "NV"  : "",
			 attr & EFI_MEMORY_XP      ? "XP"  : "",
			 attr & EFI_MEMORY_RP      ? "RP"  : "",
			 attr & EFI_MEMORY_WP      ? "WP"  : "",
			 attr & EFI_MEMORY_RO      ? "RO"  : "",
			 attr & EFI_MEMORY_UCE     ? "UCE" : "",
			 attr & EFI_MEMORY_WB      ? "WB"  : "",
			 attr & EFI_MEMORY_WT      ? "WT"  : "",
			 attr & EFI_MEMORY_WC      ? "WC"  : "",
			 attr & EFI_MEMORY_UC      ? "UC"  : "");
	return buf;
}
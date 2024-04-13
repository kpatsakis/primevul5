static void smb1cli_req_flags(enum protocol_types protocol,
			      uint32_t smb1_capabilities,
			      uint8_t smb_command,
			      uint8_t additional_flags,
			      uint8_t clear_flags,
			      uint8_t *_flags,
			      uint16_t additional_flags2,
			      uint16_t clear_flags2,
			      uint16_t *_flags2)
{
	uint8_t flags = 0;
	uint16_t flags2 = 0;

	if (protocol >= PROTOCOL_LANMAN1) {
		flags |= FLAG_CASELESS_PATHNAMES;
		flags |= FLAG_CANONICAL_PATHNAMES;
	}

	if (protocol >= PROTOCOL_LANMAN2) {
		flags2 |= FLAGS2_LONG_PATH_COMPONENTS;
		flags2 |= FLAGS2_EXTENDED_ATTRIBUTES;
	}

	if (protocol >= PROTOCOL_NT1) {
		flags2 |= FLAGS2_IS_LONG_NAME;

		if (smb1_capabilities & CAP_UNICODE) {
			flags2 |= FLAGS2_UNICODE_STRINGS;
		}
		if (smb1_capabilities & CAP_STATUS32) {
			flags2 |= FLAGS2_32_BIT_ERROR_CODES;
		}
		if (smb1_capabilities & CAP_EXTENDED_SECURITY) {
			flags2 |= FLAGS2_EXTENDED_SECURITY;
		}
	}

	flags |= additional_flags;
	flags &= ~clear_flags;
	flags2 |= additional_flags2;
	flags2 &= ~clear_flags2;

	*_flags = flags;
	*_flags2 = flags2;
}

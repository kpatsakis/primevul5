static bool validate_hash(uuid_t type, int size)
{
        if (uuid_equals(&type, &EFI_CERT_SHA1_GUID) && (size == 20))
                return true;

        if (uuid_equals(&type, &EFI_CERT_SHA224_GUID) && (size == 28))
                return true;

        if (uuid_equals(&type, &EFI_CERT_SHA256_GUID) && (size == 32))
                return true;

        if (uuid_equals(&type, &EFI_CERT_SHA384_GUID) && (size == 48))
                return true;

        if (uuid_equals(&type, &EFI_CERT_SHA512_GUID) && (size == 64))
                return true;

        return false;
}
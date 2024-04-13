    saml2md::MetadataProvider* SHIBSP_DLLLOCAL DynamicMetadataProviderFactory(const DOMElement* const & e)
    {
        return new DynamicMetadataProvider(e);
    }

EntityDescriptor* DynamicMetadataProvider::resolve(const Criteria& criteria) const
{
    string name;
    if (criteria.entityID_ascii) {
        name = criteria.entityID_ascii;
    }
    else if (criteria.entityID_unicode) {
        auto_ptr_char temp(criteria.entityID_unicode);
        name = temp.get();
    }
    else if (criteria.artifact) {
        throw MetadataException("Unable to resolve metadata dynamically from an artifact.");
    }

    try {
        DOMDocument* doc=nullptr;
        auto_ptr_XMLCh widenit(name.c_str());
        URLInputSource src(widenit.get());
        Wrapper4InputSource dsrc(&src,false);
        if (m_validate)
            doc=XMLToolingConfig::getConfig().getValidatingParser().parse(dsrc);
        else
            doc=XMLToolingConfig::getConfig().getParser().parse(dsrc);

        XercesJanitor<DOMDocument> docjanitor(doc);

        auto_ptr<XMLObject> xmlObject(XMLObjectBuilder::buildOneFromElement(doc->getDocumentElement(), true));
        docjanitor.release();

        EntityDescriptor* entity = dynamic_cast<EntityDescriptor*>(xmlObject.get());
        if (!entity) {
            throw MetadataException(
                "Root of metadata instance not recognized: $1", params(1,xmlObject->getElementQName().toString().c_str())
                );
        }
        xmlObject.release();
        return entity;
    }
    catch (XMLException& e) {
        auto_ptr_char msg(e.getMessage());
        Category::getInstance(SAML_LOGCAT ".MetadataProvider.Dynamic").error(
            "Xerces error while resolving entityID (%s): %s", name.c_str(), msg.get()
            );
        throw MetadataException(msg.get());
    }
}

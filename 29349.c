saml2md::EntityDescriptor* DynamicMetadataProvider::resolve(const saml2md::MetadataProvider::Criteria& criteria) const
{
#ifdef _DEBUG
    xmltooling::NDC("resolve");
#endif
    Category& log=Category::getInstance(SHIBSP_LOGCAT ".MetadataProvider.Dynamic");

    string name;
    if (criteria.entityID_ascii) {
        name = criteria.entityID_ascii;
    }
    else if (criteria.entityID_unicode) {
        auto_ptr_char temp(criteria.entityID_unicode);
        name = temp.get();
    }
    else if (criteria.artifact) {
        if (m_subst.empty() && (m_regex.empty() || m_match.empty()))
            throw saml2md::MetadataException("Unable to resolve metadata dynamically from an artifact.");
        name = "{sha1}" + criteria.artifact->getSource();
    }

    if (!m_subst.empty()) {
        string name2(name);
        if (!m_hashed.empty()) {
            name2 = SecurityHelper::doHash(m_hashed.c_str(), name.c_str(), name.length());
        }
        name2 = boost::replace_first_copy(m_subst, "$entityID",
            m_encoded ? XMLToolingConfig::getConfig().getURLEncoder()->encode(name2.c_str()) : name2);
        log.info("transformed location from (%s) to (%s)", name.c_str(), name2.c_str());
        name = name2;
    }
    else if (!m_match.empty() && !m_regex.empty()) {
        try {
            RegularExpression exp(m_match.c_str());
            XMLCh* temp = exp.replace(name.c_str(), m_regex.c_str());
            if (temp) {
                auto_ptr_char narrow(temp);
                XMLString::release(&temp);

                if (name != narrow.get()) {
                    log.info("transformed location from (%s) to (%s)", name.c_str(), narrow.get());
                    name = narrow.get();
                }
            }
        }
        catch (XMLException& ex) {
            auto_ptr_char msg(ex.getMessage());
            log.error("caught error applying regular expression: %s", msg.get());
        }
    }

    if (XMLString::startsWithI(name.c_str(), "file://")) {
        MetadataProvider::Criteria baseCriteria(name.c_str());
        return saml2md::DynamicMetadataProvider::resolve(baseCriteria);
    }

    const MetadataProviderCriteria* mpc = dynamic_cast<const MetadataProviderCriteria*>(&criteria);
    if (!mpc)
        throw saml2md::MetadataException("Dynamic MetadataProvider requires Shibboleth-aware lookup criteria, check calling code.");
    const PropertySet* relyingParty;
    if (criteria.artifact)
        relyingParty = mpc->application.getRelyingParty((XMLCh*)nullptr);
    else if (criteria.entityID_unicode)
        relyingParty = mpc->application.getRelyingParty(criteria.entityID_unicode);
    else {
        auto_ptr_XMLCh temp2(name.c_str());
        relyingParty = mpc->application.getRelyingParty(temp2.get());
    }

    SOAPTransport::Address addr(relyingParty->getString("entityID").second, name.c_str(), name.c_str());
    const char* pch = strchr(addr.m_endpoint,':');
    if (!pch)
        throw IOException("location was not a URL.");
    string scheme(addr.m_endpoint, pch-addr.m_endpoint);
    boost::scoped_ptr<SOAPTransport> transport;
    try {
        transport.reset(XMLToolingConfig::getConfig().SOAPTransportManager.newPlugin(scheme.c_str(), addr));
    }
    catch (exception& ex) {
        log.error("exception while building transport object to resolve URL: %s", ex.what());
        throw IOException("Unable to resolve entityID with a known transport protocol.");
    }

    transport->setVerifyHost(m_verifyHost);
    if (m_trust.get() && m_dummyCR.get() && !transport->setTrustEngine(m_trust.get(), m_dummyCR.get()))
        throw IOException("Unable to install X509TrustEngine into transport object.");

    Locker credlocker(nullptr, false);
    CredentialResolver* credResolver = nullptr;
    pair<bool,const char*> authType=relyingParty->getString("authType");
    if (!authType.first || !strcmp(authType.second,"TLS")) {
        credResolver = mpc->application.getCredentialResolver();
        if (credResolver)
            credlocker.assign(credResolver);
        if (credResolver) {
            CredentialCriteria cc;
            cc.setUsage(Credential::TLS_CREDENTIAL);
            authType = relyingParty->getString("keyName");
            if (authType.first)
                cc.getKeyNames().insert(authType.second);
            const Credential* cred = credResolver->resolve(&cc);
            cc.getKeyNames().clear();
            if (cred) {
                if (!transport->setCredential(cred))
                    log.error("failed to load Credential into metadata resolver");
            }
            else {
                log.error("no TLS credential supplied");
            }
        }
        else {
            log.error("no CredentialResolver available for TLS");
        }
    }
    else {
        SOAPTransport::transport_auth_t type=SOAPTransport::transport_auth_none;
        pair<bool,const char*> username=relyingParty->getString("authUsername");
        pair<bool,const char*> password=relyingParty->getString("authPassword");
        if (!username.first || !password.first)
            log.error("transport authType (%s) specified but authUsername or authPassword was missing", authType.second);
        else if (!strcmp(authType.second,"basic"))
            type = SOAPTransport::transport_auth_basic;
        else if (!strcmp(authType.second,"digest"))
            type = SOAPTransport::transport_auth_digest;
        else if (!strcmp(authType.second,"ntlm"))
            type = SOAPTransport::transport_auth_ntlm;
        else if (!strcmp(authType.second,"gss"))
            type = SOAPTransport::transport_auth_gss;
        else if (strcmp(authType.second,"none"))
            log.error("unknown authType (%s) specified for RelyingParty", authType.second);
        if (type > SOAPTransport::transport_auth_none) {
            if (transport->setAuth(type,username.second,password.second))
                log.debug("configured for transport authentication (method=%s, username=%s)", authType.second, username.second);
            else
                log.error("failed to configure transport authentication (method=%s)", authType.second);
        }
    }

    pair<bool,unsigned int> timeout = relyingParty->getUnsignedInt("connectTimeout");
    transport->setConnectTimeout(timeout.first ? timeout.second : 10);
    timeout = relyingParty->getUnsignedInt("timeout");
    transport->setTimeout(timeout.first ? timeout.second : 20);
    mpc->application.getServiceProvider().setTransportOptions(*transport);

    HTTPSOAPTransport* http = dynamic_cast<HTTPSOAPTransport*>(transport.get());
    if (http) {
        pair<bool,bool> flag = relyingParty->getBool("chunkedEncoding");
        http->useChunkedEncoding(flag.first && flag.second);
        http->setRequestHeader("Xerces-C", XERCES_FULLVERSIONDOT);
        http->setRequestHeader("XML-Security-C", XSEC_FULLVERSIONDOT);
        http->setRequestHeader("OpenSAML-C", gOpenSAMLDotVersionStr);
        http->setRequestHeader(PACKAGE_NAME, PACKAGE_VERSION);
    }

    try {
        transport->send();
        istream& msg = transport->receive();

        DOMDocument* doc=nullptr;
        StreamInputSource src(msg, "DynamicMetadataProvider");
        Wrapper4InputSource dsrc(&src,false);
        if (m_validate)
            doc=XMLToolingConfig::getConfig().getValidatingParser().parse(dsrc);
        else
            doc=XMLToolingConfig::getConfig().getParser().parse(dsrc);

        XercesJanitor<DOMDocument> docjanitor(doc);

        if (!doc->getDocumentElement() || !XMLHelper::isNodeNamed(doc->getDocumentElement(),
                samlconstants::SAML20MD_NS, saml2md::EntityDescriptor::LOCAL_NAME)) {
            throw saml2md::MetadataException("Root of metadata instance was not an EntityDescriptor");
        }

        auto_ptr<XMLObject> xmlObject(XMLObjectBuilder::buildOneFromElement(doc->getDocumentElement(), true));
        docjanitor.release();

        saml2md::EntityDescriptor* entity = dynamic_cast<saml2md::EntityDescriptor*>(xmlObject.get());
        if (!entity) {
            throw saml2md::MetadataException(
                "Root of metadata instance not recognized: $1", params(1,xmlObject->getElementQName().toString().c_str())
                );
        }
        xmlObject.release();
        return entity;
    }
    catch (XMLException& e) {
        auto_ptr_char msg(e.getMessage());
        log.error("Xerces error while resolving location (%s): %s", name.c_str(), msg.get());
        throw saml2md::MetadataException(msg.get());
    }
}

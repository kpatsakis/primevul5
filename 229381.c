virSecuritySELinuxSetInputLabel(virSecurityManager *mgr,
                                virDomainDef *def,
                                virDomainInputDef *input)
{
    virSecurityLabelDef *seclabel;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    switch ((virDomainInputType)input->type) {
    case VIR_DOMAIN_INPUT_TYPE_PASSTHROUGH:
    case VIR_DOMAIN_INPUT_TYPE_EVDEV:
        if (virSecuritySELinuxSetFilecon(mgr, input->source.evdev,
                                         seclabel->imagelabel, true) < 0)
            return -1;
        break;

    case VIR_DOMAIN_INPUT_TYPE_MOUSE:
    case VIR_DOMAIN_INPUT_TYPE_TABLET:
    case VIR_DOMAIN_INPUT_TYPE_KBD:
    case VIR_DOMAIN_INPUT_TYPE_LAST:
        break;
    }

    return 0;
}
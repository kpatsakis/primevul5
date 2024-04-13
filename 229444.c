virSecuritySELinuxSetTPMFileLabel(virSecurityManager *mgr,
                                  virDomainDef *def,
                                  virDomainTPMDef *tpm)
{
    int rc;
    virSecurityLabelDef *seclabel;
    char *cancel_path;
    const char *tpmdev;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    switch (tpm->type) {
    case VIR_DOMAIN_TPM_TYPE_PASSTHROUGH:
        tpmdev = tpm->data.passthrough.source.data.file.path;
        rc = virSecuritySELinuxSetFilecon(mgr, tpmdev, seclabel->imagelabel, false);
        if (rc < 0)
            return -1;

        if ((cancel_path = virTPMCreateCancelPath(tpmdev)) != NULL) {
            rc = virSecuritySELinuxSetFilecon(mgr,
                                              cancel_path,
                                              seclabel->imagelabel, false);
            VIR_FREE(cancel_path);
            if (rc < 0) {
                virSecuritySELinuxRestoreTPMFileLabelInt(mgr, def, tpm);
                return -1;
            }
        } else {
            return -1;
        }
        break;
    case VIR_DOMAIN_TPM_TYPE_EMULATOR:
        tpmdev = tpm->data.emulator.source.data.nix.path;
        rc = virSecuritySELinuxSetFilecon(mgr, tpmdev, seclabel->imagelabel, false);
        if (rc < 0)
            return -1;
        break;
    case VIR_DOMAIN_TPM_TYPE_LAST:
        break;
    }

    return 0;
}
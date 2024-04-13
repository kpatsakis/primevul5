virSecuritySELinuxRestoreTPMFileLabelInt(virSecurityManager *mgr,
                                         virDomainDef *def,
                                         virDomainTPMDef *tpm)
{
    int rc = 0;
    virSecurityLabelDef *seclabel;
    char *cancel_path;
    const char *tpmdev;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    switch (tpm->type) {
    case VIR_DOMAIN_TPM_TYPE_PASSTHROUGH:
        tpmdev = tpm->data.passthrough.source.data.file.path;
        rc = virSecuritySELinuxRestoreFileLabel(mgr, tpmdev, false);

        if ((cancel_path = virTPMCreateCancelPath(tpmdev)) != NULL) {
            if (virSecuritySELinuxRestoreFileLabel(mgr, cancel_path, false) < 0)
                rc = -1;
            VIR_FREE(cancel_path);
        }
        break;
    case VIR_DOMAIN_TPM_TYPE_EMULATOR:
        /* swtpm will have removed the Unix socket upon termination */
    case VIR_DOMAIN_TPM_TYPE_LAST:
        break;
    }

    return rc;
}
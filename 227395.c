static u8 append_appearance(struct hci_dev *hdev, u8 *ptr, u8 ad_len)
{
	return eir_append_le16(ptr, ad_len, EIR_APPEARANCE, hdev->appearance);
}
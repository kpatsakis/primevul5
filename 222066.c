static int match_filter(const struct USBAutoFilter *f1, 
                        const struct USBAutoFilter *f2)
{
    return f1->bus_num    == f2->bus_num &&
           f1->addr       == f2->addr &&
           f1->vendor_id  == f2->vendor_id &&
           f1->product_id == f2->product_id;
}
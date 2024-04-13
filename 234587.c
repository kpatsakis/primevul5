bool test_if_use_dynamic_range_scan(JOIN_TAB *join_tab)
{
    return (join_tab->use_quick == 2 && test_if_quick_select(join_tab) > 0);
}
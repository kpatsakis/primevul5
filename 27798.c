 void FormAssociatedElement::formRemovedFromTree(const Node* formRoot)
 {
     ASSERT(m_form);
    if (toHTMLElement(this)->highestAncestor() != formRoot)
        setForm(0);
 }

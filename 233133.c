ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (int)STRLEN(eap->arg), eap->cmdidx == CMD_spellwrong,
				   eap->forceit ? 0 : (int)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}
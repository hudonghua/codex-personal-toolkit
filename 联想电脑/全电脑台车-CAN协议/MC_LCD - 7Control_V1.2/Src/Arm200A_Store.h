#ifndef __ARM200A_STORE_H__
#define __ARM200A_STORE_H__

/*
 * Arm200A EEPROM store.
 * Save / load:
 * - zero-layer config
 * - zero-repeatability samples
 * - machining-error samples
 * - fix values
 * - fit summaries
 */

void Arm200A_StoreLoad(void);
void Arm200A_StoreSave(void);

#endif

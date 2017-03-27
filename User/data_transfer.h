#ifndef __DATA_TRANSFER_H
#define	__DATA_TRANSFER_H

#include "stm32f10x.h"

void Data_trans_Init(void);
void Receive_Handle(u8 ch);
void bluetooth_handle(u8 ch);
void dataupload(void);

extern int usartsendswitch;

#endif /* __DATA_TRANSFER_H */

#ifndef __ADRVMODE_H
#define __ADRVMODE_H

//
// ä÷êî
//
void adrv_mode_init();
void adrv_mode_change_status();
void process_on_one_second_adrv_mode();
void process_on_adrv_mode();
void reverse_on_adrv_mode();
void adrv_mode_sense_reducing();
void adrv_mode_sense_stopping();

#endif // __ADRVMODE_H

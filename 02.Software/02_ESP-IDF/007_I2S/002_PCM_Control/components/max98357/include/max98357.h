#ifndef _MAX98357_H
#define _MAX98357_H

extern volatile bool is_paused;
extern TaskHandle_t i2sHandle;

void i2s_write_task(void *param);
void i2s_init(void);


#endif
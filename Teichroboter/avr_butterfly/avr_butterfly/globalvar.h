/*
 * globalvar.h
 *
 * Created: 02.06.2013 08:30:32
 *  Author: mirko
 */ 


#ifndef GLOBALVAR_H_
#define GLOBALVAR_H_

extern volatile int32_t time_ms;
extern volatile int16_t time_sec;
extern uint8_t PowerSave;
extern uint8_t akku_niedrig;
extern uint8_t SleepIdle;
extern int8_t rechts_pwm;
extern int8_t links_pwm;
extern uint8_t pumpe_ein;
extern int16_t kompass_grad;
extern uint16_t akku_spannung;
extern uint8_t anlage_aus;
extern uint8_t pumpe_ausgeklappt;
extern int8_t temperatur;
extern int16_t kurs_plan;
extern int16_t kurs_akt;
extern int16_t kurs_diff;
extern int8_t geschwindigkeit;
extern uint8_t statustextnr;
extern uint8_t antriebsfreigabe;
extern uint8_t init;
extern int16_t drehversuche;

#endif /* GLOBALVAR_H_ */
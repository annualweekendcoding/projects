#ifndef _RFM12_LAYER2_H
#define _RFM12_LAYER2_H

#include <avr/io.h>

enum rfm12_tsend_state 
{
  send_idle,      // Es wird gerade nicht gesendet und kein Fehler
  send_buffered,  // Daten stehen im Puffer
  send_wait,      // Warten auf Bestätigung
  send_error,     // Beim letzten Senden trat ein Fehler auf
  send_ok         // Senden erfolgreich und Bestätigung erhalten
};

extern enum rfm12_tsend_state rfm12_send_state;

enum rfm12_tsend_error 
{
  send_none,     // Kein Fehler
  send_hardware, // RFM12-Modul gestört
  send_nack,     // Ablehnung erhalten
  send_timeout   // Bestätigung nicht erhalten
};

extern enum rfm12_tsend_error rfm12_send_lasterror;

// Lesen und Setzen der maximalen Wiederholung beim Senden
extern uint8_t rfm12_send_retry_limit;

enum rfm12_treceive_state 
{
  receive_idle, // Es wird auf Empfang gewartet und kein Fehler
  receive_data  // Es sind Empfangene Daten vorhanden
};

extern enum rfm12_treceive_state rfm12_receive_state;

enum rfm12_treceive_error 
{
  receive_none,     // Kein Fehler
  receive_hardware, // RFM12-Modul gestört
  receive_crc,      // Prüfsummenfehler in den Daten
};

extern enum rfm12_treceive_error rfm12_receive_lasterror;

#define RFM12_TELTYPE_DATANACK 1  // nicht zu bestätigenden Datenpaket
#define RFM12_TELTYPE_DATA     2  // zu bestätigenden Datenpaket
#define RFM12_TELTYPE_ACK      3  // Bestätigungspaket
#define RFM12_TELTYPE_NACK     4  // Ablehnung
#define RFM12_TELTYPE_PING     5  // Ping-Paket
#define RFM12_TELTYPE_PINGACK  6  // Antwort auf Ping-Paket
#define RFM12_TELTYPE_FRAGMENT 7  // Fragment mit laufender Nummer

// Initialisieren
void rfm12_layer2_init();

// Senden an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
uint8_t rfm12_send(uint16_t address, uint8_t teltype, uint8_t *data, uint8_t len);

// Senden eines Datenblocks mit Bestätigung an eine Adresse
static inline uint8_t rfm12_send_data(uint16_t address, uint8_t *data, uint8_t len) 
{
  return rfm12_send(address,RFM12_TELTYPE_DATA,data,len);
};

// Ping an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
static inline uint8_t rfm12_ping(uint16_t address) 
{
  return rfm12_send(address,RFM12_TELTYPE_PING,0,0);
};

struct rfm12_treceive_buffer
{
  uint16_t address; // Adresse des Absenders
  uint8_t len;      // Länge der Daten
  uint8_t *data;  // Zeiger auf die Daten
} extern rfm12_receive_buffer;

// Freigeben des Empfangspuffers für neue Daten
void rfm12_receive_clear(); 

void rfm12_layer2_tick (void);

#endif //_RFM12_LAYER2_H


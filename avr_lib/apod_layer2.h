#ifndef _APOD_LAYER2_H
#define _APOD_LAYER2_H

#include <avr/io.h>

#define APOD_BUFFER_SIZE 20

#ifndef APOD_MYADDRESS
#define APOD_MYADDRESS 1
#endif

#ifndef APOD_BROADCAST_ADDRESS
#define APOD_BROADCAST_ADDRESS 1
#endif

enum apod_tsend_state 
{
  send_idle,      // Es wird gerade nicht gesendet und kein Fehler
  send_buffered,  // Daten stehen im Puffer
  send_header,    // Header gesendet
  send_wait,      // Warten auf Bestätigung
  send_error,     // Beim letzten Senden trat ein Fehler auf
  send_ok         // Senden erfolgreich und Bestätigung erhalten
};

extern enum apod_tsend_state apod_send_state;

enum apod_tsend_error 
{
  send_none,     // Kein Fehler
  send_hardware, // APOD-Modul gestört
  send_nack,     // Ablehnung erhalten
  send_timeout   // Bestätigung nicht erhalten
};

extern enum apod_tsend_error apod_send_lasterror;

// Lesen und Setzen der maximalen Wiederholung beim Senden
extern uint8_t apod_send_retry_limit;

enum apod_treceive_state 
{
  receive_sync, // Es wird auf das Sync-Zeichen gewartet und kein Fehler
  receive_idle, // Es wird auf Empfang gewartet und kein Fehler
  receive_header,  // Es wird der Header empfangen
  receive_data,  // Es werden die Daten empfangen
  receive_crc,  // Es wird die CRC empfangen
  receive_complete,  // Es wurde ein Telegramm vollständig empfangen
  receive_data  // Es sind Empfangene Daten vorhanden
};

extern enum apod_treceive_state apod_receive_state;

enum apod_treceive_error 
{
  receive_none,     // Kein Fehler
  receive_crc,      // Prüfsummenfehler
};

extern enum apod_treceive_error apod_receive_lasterror;

#define APOD_TELTYPE_DATANACK 1  // nicht zu bestätigenden Datenpaket
#define APOD_TELTYPE_DATA     2  // zu bestätigenden Datenpaket
#define APOD_TELTYPE_ACK      3  // Bestätigungspaket
#define APOD_TELTYPE_NACK     4  // Ablehnung
#define APOD_TELTYPE_PING     5  // Ping-Paket
#define APOD_TELTYPE_PINGACK  6  // Antwort auf Ping-Paket
#define APOD_TELTYPE_FRAGMENT 7  // Fragment mit laufender Nummer

// Initialisieren
void apod_layer2_init();

// Senden an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
uint8_t apod_send(uint16_t address, uint8_t teltype, uint8_t *data, uint8_t len);

// Senden eines Datenblocks mit Bestätigung an eine Adresse
static inline uint8_t apod_send_data(uint16_t address, uint8_t *data, uint8_t len) 
{
  return apod_send(address,APOD_TELTYPE_DATA,data,len);
};

// Ping an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
static inline uint8_t apod_ping(uint16_t address) 
{
  return apod_send(address,APOD_TELTYPE_PING,0,0);
};

typedef struct apod_tel_buffer
{
  uint8_t teltype;        // Telegrammtyp (siehe APOD_TELTYPE_* ) oder 0 für Puffer frei
  uint16_t srcaddress;    // Quelladresse
  uint16_t destaddress;   // Zieladresse
  uint8_t telcount;       // Telegrammzähler
  uint8_t len;            // Datenlänge
  uint8_t crc;            // CRC-Prüfsumme über Kopf und Daten
  uint8_t data[APOD_BUFFER_SIZE]; // Puffer für die Daten
} extern apod_receive_buffer;

#define APOD_HEADER_SIZE = sizeof(apod_tel_buffer)-APOD_BUFFER_SIZE; // Struktur vor den Daten in obiger Struktur

// Freigeben des Empfangspuffers für neue Daten
void apod_receive_clear(); 

void apod_layer2_tick (void);

#endif //_APOD_LAYER2_H


#include <string.h>
#include "rfm12_layer2.h"
#include "rfm12.h"
#include "timer.h"

// Timeout in ms bis gesendet und Bestätigung empfangen sein muss
#define RFM12_ACK_TIMEOUT 1000
// Timeout in ms bis Telegramm an Sendepuffer übergeben wird
#define RFM12_SEND_QUEUE_TIMEOUT 1000


// Deklaration der Variablen aus der Header-Datei
enum rfm12_tsend_state rfm12_send_state; 
enum rfm12_tsend_error rfm12_send_lasterror; 

// Standardmaessige Wiederholung einstellen
uint8_t rfm12_send_retry_limit=3;

enum rfm12_treceive_state rfm12_receive_state;
enum rfm12_treceive_error rfm12_receive_lasterror;
struct rfm12_treceive_buffer rfm12_receive_buffer;

struct rfm12_tsend_header_buffer
{
  uint8_t teltype;        // Telegrammtyp (siehe RFM12_TELTYPE_* )
  uint16_t srcaddress;    // Quelladresse
  uint16_t destaddress;   // Zieladresse
  uint8_t telcount;       // Telegrammzähler
} rfm12_send_header_buffer;

#define RFM12_SEND_HEADER_SIZE sizeof(rfm12_send_header_buffer)

typedef struct 
{
  uint8_t teltype;        // Telegrammtyp (siehe RFM12_TELTYPE_* ) oder 0 für Puffer frei
  uint16_t srcaddress;    // Quelladresse
  uint16_t destaddress;   // Zieladresse
  uint8_t telcount;       // Telegrammzähler
  uint8_t data[RFM12_TX_BUFFER_SIZE-RFM12_SEND_HEADER_SIZE]; // Puffer für die Daten
  uint8_t len;            // Datenlänge (wird nicht mit gesendet)
  uint8_t retry_count;    // Anzahl der Wiederholungen
} rfm12_tsend_buffer;

rfm12_tsend_buffer rfm12_send_buffer;

// Sendezeit für Zeitüberwachung
int rfm12_send_time;

// Initialisierung
void rfm12_layer2_init()
{
  rfm12_send_state = send_idle;
  rfm12_receive_state = receive_idle;
  rfm12_init();
}

// Senden eines Datenblocks an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
uint8_t rfm12_send(uint16_t address, uint8_t teltype, uint8_t *data, uint8_t len)
{
  // Sendepuffer ist noch in Arbeit
  if (rfm12_send_state==send_buffered) return 0;
  // Im Moment noch ohne Fragmente, zu große Pakete abweisen
  if (len>RFM12_TX_BUFFER_SIZE-RFM12_SEND_HEADER_SIZE) return 0;
  rfm12_send_buffer.srcaddress = RFM12_MYADDRESS;
  rfm12_send_buffer.telcount++;
  rfm12_send_buffer.retry_count = 0;
  rfm12_send_buffer.teltype = teltype;
  rfm12_send_buffer.destaddress = address;
  if (len && data) memcpy(rfm12_send_buffer.data,data,len);
  rfm12_send_buffer.len=len;
  rfm12_send_state = send_buffered;
  rfm12_send_time = time_ms;
  return 1;
}

// Freigeben des Empfangspuffers für neue Daten
void rfm12_receive_clear()
{
  rfm12_receive_buffer.address = 0;
  rfm12_receive_buffer.len = 0;
  rfm12_receive_state = receive_idle;
  rfm12_rx_clear(); // unterlagerten Puffer wieder freigeben
}

// Zyklisches Programm
void rfm12_layer2_tick()
{
  if (rfm12_send_state == send_buffered)
  {
    // Es ist etwas zum Senden vorhanden
    uint8_t retval = rfm12_tx (RFM12_MYTYPE, (uint8_t*)&rfm12_send_buffer, rfm12_send_buffer.len+RFM12_SEND_HEADER_SIZE);
    if (retval==0x80)
    {
      // erfolgreich eingetragen
      rfm12_send_state = send_wait;
      rfm12_send_time = time_ms;
      debug_write_Pstr(PSTR("Packet Enqeued\r\n"));
    }
    else if ((time_ms-rfm12_send_time)>RFM12_SEND_QUEUE_TIMEOUT)
    {
      // Timeout beim Warten auf die Sendemöglichkeit
      // Fehlerzustand einnehmen
      rfm12_send_state = send_error;
      rfm12_send_lasterror = send_hardware;
      debug_write_Pstr(PSTR("SEND QUEUE TIMEOUT\r\n"));
    }
  }

  rfm12_tick();

  if (rfm12_rx_status() == RFM12_STATUS_COMPLETE)
	{
    uint8_t rx_no_clear=0;
    if ((rfm12_rx_type()!=RFM12_MYTYPE) || (rfm12_rx_len()<RFM12_SEND_HEADER_SIZE))
    {
      debug_write_Pstr(PSTR("REC INVALID\r\n"));
      // Paket aus anderem Protokoll oder zu kleines Paket ignorieren
    }
    else
    {
      // Flag fuer den Sendeanstoss für Antwortpakete
      uint8_t send_header_buffer=0;
  		rfm12_tsend_buffer *buf = (rfm12_tsend_buffer*)rfm12_rx_buffer();
      if (buf->teltype==RFM12_TELTYPE_DATANACK || (buf->destaddress==RFM12_BROADCAST_ADDRESS && buf->teltype==RFM12_TELTYPE_DATA))
      {
        // Broadcast Empfang melden und nicht bestätigen
        debug_write_Pstr(PSTR("REC BROADCAST\r\n"));
        if (rfm12_receive_state == receive_idle)
        {
          rfm12_receive_buffer.address = buf->srcaddress;
          rfm12_receive_buffer.len = rfm12_rx_len()-RFM12_SEND_HEADER_SIZE;
          rfm12_receive_buffer.data = buf->data;
          rfm12_receive_state = receive_data;
          rx_no_clear = 1;
        }
      }
      else if (buf->destaddress==RFM12_MYADDRESS)
      {
        // Paket für meine Adresse
        switch (buf->teltype)
        {
          case RFM12_TELTYPE_DATA :
            // Empfang melden und bestätigen
            debug_write_Pstr(PSTR("REC DATA\r\n"));
            if (rfm12_receive_state == receive_idle)
            {
              rfm12_receive_buffer.address = buf->srcaddress;
              rfm12_receive_buffer.len = rfm12_rx_len()-RFM12_SEND_HEADER_SIZE;
              rfm12_receive_buffer.data = buf->data;
              rfm12_receive_state = receive_data;
              rx_no_clear = 1;
              // Antwortpaket zusammenstellen
              rfm12_send_header_buffer.teltype = RFM12_TELTYPE_ACK;
              rfm12_send_header_buffer.destaddress = buf->srcaddress;
              rfm12_send_header_buffer.telcount = buf->telcount;
              send_header_buffer=1;
            }
            break;

          case RFM12_TELTYPE_ACK :
            debug_write_Pstr(PSTR("REC ACK\r\n"));
            if (rfm12_send_state==send_wait
              && rfm12_send_buffer.destaddress==buf->srcaddress
              && rfm12_send_buffer.telcount==buf->telcount)
            {
              // Korrekte Bestätigung erhalten, Senden ist damit erfolgreich beendet
              rfm12_send_state = send_ok;
            }
            break;

          case RFM12_TELTYPE_NACK :
            debug_write_Pstr(PSTR("REC NACK\r\n"));
            if (rfm12_send_state==send_wait
              && rfm12_send_buffer.destaddress==buf->srcaddress
              && rfm12_send_buffer.telcount==buf->telcount)
            {
              // Ablehnung erhalten, Senden ist damit mit Fehler beendet
              rfm12_send_state = send_error;
              rfm12_send_lasterror = send_nack;
            }
            break;

          case RFM12_TELTYPE_PING :
            // Antwortpaket zusammenstellen
            debug_write_Pstr(PSTR("REC PING\r\n"));
            rfm12_send_header_buffer.teltype = RFM12_TELTYPE_PINGACK;
            rfm12_send_header_buffer.destaddress = buf->srcaddress;
            rfm12_send_header_buffer.telcount = buf->telcount;
            send_header_buffer=1;
            break;

          case RFM12_TELTYPE_PINGACK :
            debug_write_Pstr(PSTR("REC PINGACK\r\n"));
            if (rfm12_send_state==send_wait
              && rfm12_send_buffer.destaddress==buf->srcaddress
              && rfm12_send_buffer.telcount==buf->telcount
              && rfm12_send_buffer.teltype==RFM12_TELTYPE_PING)
            {
              // Korrekte Bestätigung erhalten, Ping ist damit erfolgreich beendet
              rfm12_send_state = send_ok;
            }
            break;

          case RFM12_TELTYPE_FRAGMENT :
            // noch nicht implementiert
            break;
          default:
            // alles andere wird ignoriert
            break;
        }
        if (send_header_buffer)
        {
          // Ein Antworttelegramm soll gesendet werden
          rfm12_send_header_buffer.srcaddress = RFM12_MYADDRESS;
          // Telegramm senden, der Rückgabewert ist uns hier egal, weil man daraus nichts ableiten kann
          rfm12_tx (RFM12_MYTYPE, (uint8_t*)&rfm12_send_header_buffer, RFM12_SEND_HEADER_SIZE);
          debug_write_Pstr(PSTR("SEND HEADER\r\n"));
        }
      } // if MYADDRESS
    }
    if (!rx_no_clear) rfm12_rx_clear(); // unterlagerten Puffer wieder freigeben
  } // if received

  if (rfm12_send_state==send_wait && (time_ms-rfm12_send_time)>RFM12_ACK_TIMEOUT)
  {
    // Timeout beim Warten auf Sendebestätigung abgelaufen
    debug_write_Pstr(PSTR("SEND TIMEOUT\r\n"));
    if (rfm12_send_buffer.retry_count++ < rfm12_send_retry_limit)
    {
      rfm12_tx (RFM12_MYTYPE, (uint8_t*)&rfm12_send_buffer, rfm12_send_buffer.len+RFM12_SEND_HEADER_SIZE);
      rfm12_send_time=time_ms;
    }
    else
    {
      rfm12_send_state=send_error;
      rfm12_send_lasterror = send_timeout;
    }
  }
}

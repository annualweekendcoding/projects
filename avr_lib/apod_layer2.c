#include <string.h>
#include "apod_layer2.h"
#include "usart_raw.h"
#include "timer.h"

// Timeout in ms bis gesendet und Bestätigung empfangen sein muss
#define APOD_ACK_TIMEOUT 1000
// Timeout in ms bis Telegramm an Sendepuffer übergeben wird
#define APOD_SEND_QUEUE_TIMEOUT 1000


// Deklaration der Variablen aus der Header-Datei
enum apod_tsend_state apod_send_state;
enum apod_tsend_error apod_send_lasterror;

// Standardmaessige Wiederholung einstellen
uint8_t apod_send_retry_limit=3;

enum apod_treceive_state apod_receive_state;
enum apod_treceive_error apod_receive_lasterror;
struct apod_tel_buffer apod_receive_buffer;
struct apod_tel_buffer apod_send_buffer;

// Sendezeit für Zeitüberwachung
int apod_send_time;

// Initialisierung
void apod_layer2_init()
{
  apod_send_state = send_idle;
  apod_receive_state = receive_sync;
}

// Senden eines Datenblocks an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
uint8_t apod_send(uint16_t address, uint8_t teltype, uint8_t *data, uint8_t len)
{
  // Sendepuffer ist noch in Arbeit
  if (apod_send_state==send_buffered) return 0;
  if (len>APOD_BUFFER_SIZE) return 0;
  // Im Moment noch ohne Fragmente, zu große Pakete abweisen
  if (len>APOD_TX_BUFFER_SIZE-APOD_SEND_HEADER_SIZE) return 0;
  apod_send_buffer.srcaddress = APOD_MYADDRESS;
  apod_send_buffer.telcount++;
  apod_send_buffer.retry_count = 0;
  apod_send_buffer.teltype = teltype;
  apod_send_buffer.destaddress = address;
  if (len && data) memcpy(apod_send_buffer.data,data,len);
  apod_send_buffer.len=len;
  apod_send_state = send_buffered;
  apod_send_time = time_ms;
  return 1;
}

// Freigeben des Empfangspuffers für neue Daten
void apod_receive_clear()
{
  apod_receive_buffer.address = 0;
  apod_receive_buffer.len = 0;
  apod_receive_state = receive_idle;
}

// Zyklisches Programm
void apod_layer2_tick()
{
  if (apod_send_state == send_buffered)
  {
    // Es ist etwas zum Senden vorhanden
    uint8_t retval = usart_write_buffer((uint8_t*)&apod_send_buffer, apod_send_buffer.len+APOD_HEADER_SIZE);
    if (retval==0x80)
    uint8_t retval = usart_write_buffer((uint8_t*)&apod_send_buffer.data, );
    if (retval==0x80)
    {
      // erfolgreich eingetragen
      apod_send_state = send_wait;
      apod_send_time = time_ms;
      debug_write_Pstr(PSTR("Packet Enqeued\r\n"));
    }
    else if ((time_ms-apod_send_time)>APOD_SEND_QUEUE_TIMEOUT)
    {
      // Timeout beim Warten auf die Sendemöglichkeit
      // Fehlerzustand einnehmen
      apod_send_state = send_error;
      apod_send_lasterror = send_hardware;
      debug_write_Pstr(PSTR("SEND QUEUE TIMEOUT\r\n"));
    }
  }
  
  while ( (apod_receive_state==receive_sync) && (apod_rx_len()>0) )
  {
    // Auf Sync-Zeichen warten
    if (usart_read()==0xAA) apod_receive_state=receive_idle;
  }

  if ( (apod_receive_state==receive_idle) && (apod_rx_len()>0) )
  {
    apod_treceive_buffer.address = usart_read();
    apod_receive_state=receive_adr1;
  }
  
  if ( (apod_receive_state==receive_adr1) && (apod_rx_len()>0) )
  {
    apod_treceive_buffer.address += ((uint16_t) usart_read())<<8;
    apod_receive_state=receive_adr2;
  }
  
  if ( (apod_receive_state==receive_adr2) && (apod_rx_len()>0) )
  {
    apod_treceive_buffer.len = usart_read();
    apod_receive_state=receive_len;
  }

  while ( (apod_receive_state==receive_len) && (apod_rx_len()>0) )
  {
    apod_treceive_buffer.address = usart_read();
    apod_receive_state=receive_adr1;
  }

  if (apod_receive_state==receive_complete)
  {
    uint8_t rx_no_clear=0;
    if ((apod_rx_type()!=APOD_MYTYPE) || (apod_rx_len()<APOD_SEND_HEADER_SIZE))
    {
      debug_write_Pstr(PSTR("REC INVALID\r\n"));
      // Paket aus anderem Protokoll oder zu kleines Paket ignorieren
    }
    else
    {
      // Flag fuer den Sendeanstoss für Antwortpakete
      uint8_t send_header_buffer=0;
      apod_tsend_buffer *buf = (apod_tsend_buffer*)apod_rx_buffer();
      if (buf->teltype==APOD_TELTYPE_DATANACK || (buf->destaddress==APOD_BROADCAST_ADDRESS && buf->teltype==APOD_TELTYPE_DATA))
      {
        // Broadcast Empfang melden und nicht bestätigen
        debug_write_Pstr(PSTR("REC BROADCAST\r\n"));
        if (apod_receive_state == receive_idle)
        {
          apod_receive_buffer.address = buf->srcaddress;
          apod_receive_buffer.len = apod_rx_len()-APOD_SEND_HEADER_SIZE;
          apod_receive_buffer.data = buf->data;
          apod_receive_state = receive_data;
          rx_no_clear = 1;
        }
      }
      else if (buf->destaddress==APOD_MYADDRESS)
      {
        // Paket für meine Adresse
        switch (buf->teltype)
        {
          case APOD_TELTYPE_DATA :
          // Empfang melden und bestätigen
          debug_write_Pstr(PSTR("REC DATA\r\n"));
          if (apod_receive_state == receive_idle)
          {
            apod_receive_buffer.address = buf->srcaddress;
            apod_receive_buffer.len = apod_rx_len()-APOD_SEND_HEADER_SIZE;
            apod_receive_buffer.data = buf->data;
            apod_receive_state = receive_data;
            rx_no_clear = 1;
            // Antwortpaket zusammenstellen
            apod_send_header_buffer.teltype = APOD_TELTYPE_ACK;
            apod_send_header_buffer.destaddress = buf->srcaddress;
            apod_send_header_buffer.telcount = buf->telcount;
            send_header_buffer=1;
          }
          break;

          case APOD_TELTYPE_ACK :
          debug_write_Pstr(PSTR("REC ACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==buf->srcaddress
          && apod_send_buffer.telcount==buf->telcount)
          {
            // Korrekte Bestätigung erhalten, Senden ist damit erfolgreich beendet
            apod_send_state = send_ok;
          }
          break;

          case APOD_TELTYPE_NACK :
          debug_write_Pstr(PSTR("REC NACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==buf->srcaddress
          && apod_send_buffer.telcount==buf->telcount)
          {
            // Ablehnung erhalten, Senden ist damit mit Fehler beendet
            apod_send_state = send_error;
            apod_send_lasterror = send_nack;
          }
          break;

          case APOD_TELTYPE_PING :
          // Antwortpaket zusammenstellen
          debug_write_Pstr(PSTR("REC PING\r\n"));
          apod_send_header_buffer.teltype = APOD_TELTYPE_PINGACK;
          apod_send_header_buffer.destaddress = buf->srcaddress;
          apod_send_header_buffer.telcount = buf->telcount;
          send_header_buffer=1;
          break;

          case APOD_TELTYPE_PINGACK :
          debug_write_Pstr(PSTR("REC PINGACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==buf->srcaddress
          && apod_send_buffer.telcount==buf->telcount
          && apod_send_buffer.teltype==APOD_TELTYPE_PING)
          {
            // Korrekte Bestätigung erhalten, Ping ist damit erfolgreich beendet
            apod_send_state = send_ok;
          }
          break;

          case APOD_TELTYPE_FRAGMENT :
          // noch nicht implementiert
          break;
          default:
          // alles andere wird ignoriert
          break;
        }
        if (send_header_buffer)
        {
          // Ein Antworttelegramm soll gesendet werden
          apod_send_header_buffer.srcaddress = APOD_MYADDRESS;
          // Telegramm senden, der Rückgabewert ist uns hier egal, weil man daraus nichts ableiten kann
          apod_tx (APOD_MYTYPE, (uint8_t*)&apod_send_header_buffer, APOD_SEND_HEADER_SIZE);
          debug_write_Pstr(PSTR("SEND HEADER\r\n"));
        }
      } // if MYADDRESS
    }
    if (!rx_no_clear) apod_rx_clear(); // unterlagerten Puffer wieder freigeben
  } // if received

  if (apod_send_state==send_wait && (time_ms-apod_send_time)>APOD_ACK_TIMEOUT)
  {
    // Timeout beim Warten auf Sendebestätigung abgelaufen
    debug_write_Pstr(PSTR("SEND TIMEOUT\r\n"));
    if (apod_send_buffer.retry_count++ < apod_send_retry_limit)
    {
      apod_tx (APOD_MYTYPE, (uint8_t*)&apod_send_buffer, apod_send_buffer.len+APOD_SEND_HEADER_SIZE);
      apod_send_time=time_ms;
    }
    else
    {
      apod_send_state=send_error;
      apod_send_lasterror = send_timeout;
    }
  }
}

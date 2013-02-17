#include <string.h>
#include "apod_layer2.h"
#include "usart_raw.h"
#include "timer.h"
#include "crc8.h"

// Timeout in ms bis gesendet und Bestätigung empfangen sein muss
#define APOD_ACK_TIMEOUT 1000
// Timeout in ms bis Telegramm an Sendepuffer übergeben wird
#define APOD_SEND_QUEUE_TIMEOUT 1000

// Deklaration der Variablen aus der Header-Datei
enum apod_tsend_state apod_send_state;
enum apod_tsend_error apod_send_lasterror;

// Standardmaessige Wiederholung einstellen
uint8_t apod_send_retry_limit=3;
uint8_t apod_send_retry_count;
uint8_t apod_send_telcount=0;

enum apod_treceive_state apod_receive_state;
enum apod_treceive_error apod_receive_lasterror;
struct apod_tel_buffer apod_receive_buffer;
struct apod_tel_buffer apod_send_buffer;

uint8_t apod_receive_len;
uint8_t *apod_receive_buffer_ptr;
enum apod_treceive_error apod_receive_error;

// Sendezeit für Zeitüberwachung
int apod_send_time;

// Freigeben des Empfangspuffers für neue Daten
void apod_receive_clear()
{
  apod_receive_buffer.destaddress = 0;
  apod_receive_buffer.len = 0;
  apod_receive_state = receive_sync;
}

// Initialisierung
void apod_layer2_init()
{
  apod_send_state = send_idle;
  apod_receive_clear();
}

// Senden eines Datenblocks an eine Adresse
// in rfm_send_state kann der Vorgang verfolgt werden
// Es wird true zurückgegeben, wenn der Vorgang erfolgreich angestossen wurde
uint8_t apod_send(uint16_t address, uint8_t teltype, uint8_t *data, uint8_t len)
{
  // Sendepuffer ist noch in Arbeit
  if (apod_send_state==send_buffered) return 0;
  if (len>APOD_BUFFER_SIZE) return 0;
  apod_send_buffer.srcaddress = APOD_MYADDRESS;
  apod_send_buffer.telcount = apod_send_telcount++;
  apod_send_retry_count = 0;
  apod_send_buffer.teltype = teltype;
  apod_send_buffer.destaddress = address;
  if (len && data) memcpy(apod_send_buffer.data,data,len);
  apod_send_buffer.len=len;
  apod_send_state = send_buffered;
  apod_send_time = time_ms;
  return 1;
}

// Hilfsfunktion zum absenden mit CRC
uint8_t apod_do_send()
{
  // Den Sendepuffer absenden
  uint8_t retval = usart_write(0xAA); // SYNC senden
  if (retval==0x80)
  {
    apod_send_buffer.srcaddress = APOD_MYADDRESS;
    apod_send_buffer.crc = crc8(((uint8_t*)&apod_send_buffer)+1,apod_send_buffer.len+APOD_HEADER_SIZE-1);
    retval = usart_write_buffer((uint8_t*)&apod_send_buffer, apod_send_buffer.len+APOD_HEADER_SIZE);
  }
  return retval;
}

void apod_send_header(uint16_t address, uint8_t teltype, uint8_t telcount)
{
  // Senden eines einfachen Telegramms nur Header
  // Nur Senden wenn das Senden im idle ist.
  if (apod_send_state == send_idle)
  {
    apod_send_buffer.teltype = teltype;
    apod_send_buffer.destaddress = address;
    apod_send_buffer.telcount = telcount;
    apod_send_buffer.len=0;
    apod_do_send();
  }
}

// Zyklisches Programm
void apod_layer2_tick()
{
  if (apod_send_state == send_buffered)
  {
    // Es ist etwas zum Senden vorhanden
    uint8_t retval = apod_do_send();
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
  
  while ( (apod_receive_state==receive_sync) && (usart_read_len()>0) )
  {
    // Auf Sync-Zeichen warten
    if (usart_read()==0xAA) apod_receive_state=receive_idle;
  }

  if ( (apod_receive_state==receive_idle) && (usart_read_len()>0) )
  {
    // Sync-Zeichen weiter überlesen
    uint8_t b = usart_read();
    if (b!=0xAA)
    {
      // Ein anderes Zeichen ist das erste Zeichen im Header
      apod_receive_state=receive_progress;
      apod_receive_len=1;
      apod_receive_buffer_ptr = (uint8_t*) &apod_receive_buffer;
      *apod_receive_buffer_ptr++ = b;
    }
  }
  
  while ( (apod_receive_state==receive_progress) && (usart_read_len()>0) )
  {
    apod_receive_len++;
    if (apod_receive_len<APOD_BUFFER_SIZE+APOD_HEADER_SIZE)
    {
      *apod_receive_buffer_ptr++ = usart_read();
    }

    if (apod_receive_len>=APOD_HEADER_SIZE && apod_receive_len-APOD_HEADER_SIZE >= apod_receive_buffer.len)
    {
      // Es wurde genau der Header und die entsprechende Datenlänge empfangen
      if (apod_receive_buffer.len<APOD_BUFFER_SIZE && apod_receive_buffer.crc == crc8(((uint8_t*)&apod_receive_buffer)+1,apod_receive_buffer.len+APOD_HEADER_SIZE-1))
      {
        // Empfang komplett und CRC i.o.
        apod_receive_state=receive_complete;
        apod_receive_error=receive_none;
      }
      else
      {
        // CRC-Fehler oder Überlänge
        if (apod_receive_buffer.len<APOD_BUFFER_SIZE)
        {
          debug_write_Pstr(PSTR("REC FAIL CRC\r\n"));
          apod_receive_error=receive_crc;
        }
        else
        {
          debug_write_Pstr(PSTR("REC FAIL OVERLEN\r\n"));
          apod_receive_error=receive_overlen;
        }
        // Empfang melden und mit Fehler bestätigen
        apod_receive_state = receive_sync;
        // Antwortpaket zusammenstellen
        apod_send_header(apod_receive_buffer.srcaddress,APOD_TELTYPE_NACK,apod_receive_buffer.telcount);
      }
    }
  }

  if (apod_receive_state==receive_complete)
  {
    if (apod_receive_buffer.teltype==APOD_TELTYPE_DATANACK
    || (apod_receive_buffer.destaddress==APOD_BROADCAST_ADDRESS && apod_receive_buffer.teltype==APOD_TELTYPE_DATA))
    {
      // Broadcast Empfang melden und nicht bestätigen
      debug_write_Pstr(PSTR("REC BROADCAST\r\n"));
    }
    else if (apod_receive_buffer.destaddress==APOD_MYADDRESS)
    {
      // Paket für meine Adresse
      switch (apod_receive_buffer.teltype)
      {
        case APOD_TELTYPE_DATA :
        {
          // Empfang melden und bestätigen
          debug_write_Pstr(PSTR("REC DATA\r\n"));
          apod_receive_state = receive_data;
          // Antwortpaket zusammenstellen
          apod_send_header(apod_receive_buffer.srcaddress,APOD_TELTYPE_ACK,apod_receive_buffer.telcount);
          break;
        }

        case APOD_TELTYPE_ACK :
        {
          debug_write_Pstr(PSTR("REC ACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==apod_receive_buffer.srcaddress
          && apod_send_buffer.telcount==apod_receive_buffer.telcount)
          {
            // Korrekte Bestätigung erhalten, Senden ist damit erfolgreich beendet
            apod_send_state = send_ok;
          }
          apod_receive_state=receive_sync;
          break;
        }

        case APOD_TELTYPE_NACK :
        {
          debug_write_Pstr(PSTR("REC NACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==apod_receive_buffer.srcaddress
          && apod_send_buffer.telcount==apod_receive_buffer.telcount)
          {
            // Ablehnung erhalten, Senden ist damit mit Fehler beendet
            apod_send_state = send_error;
            apod_send_lasterror = send_nack;
          }
          apod_receive_state=receive_sync;
          break;
        }

        case APOD_TELTYPE_PING :
        {
          // Antwortpaket zusammenstellen
          debug_write_Pstr(PSTR("REC PING\r\n"));
          apod_send_header(apod_receive_buffer.srcaddress,APOD_TELTYPE_PINGACK,apod_receive_buffer.telcount);
          apod_receive_state=receive_sync;
          break;
        }

        case APOD_TELTYPE_PINGACK :
        {
          debug_write_Pstr(PSTR("REC PINGACK\r\n"));
          if (apod_send_state==send_wait
          && apod_send_buffer.destaddress==apod_receive_buffer.srcaddress
          && apod_send_buffer.telcount==apod_receive_buffer.telcount
          && apod_send_buffer.teltype==APOD_TELTYPE_PING)
          {
            // Korrekte Bestätigung erhalten, Ping ist damit erfolgreich beendet
            apod_send_state = send_ok;
          }
          break;
          apod_receive_state=receive_sync;
        }

        case APOD_TELTYPE_FRAGMENT :
        // noch nicht implementiert
        apod_receive_state=receive_sync;
        break;
        default:
        // alles andere wird ignoriert
        apod_receive_state=receive_sync;
        break;
      }
    } // if MYADDRESS
  } // if received

  if (apod_send_state==send_wait && (time_ms-apod_send_time)>APOD_ACK_TIMEOUT)
  {
    // Timeout beim Warten auf Sendebestätigung abgelaufen
    debug_write_Pstr(PSTR("SEND TIMEOUT\r\n"));
    if (apod_send_retry_count++ < apod_send_retry_limit)
    {
      apod_send_time=time_ms;
    }
    else
    {
      apod_send_state=send_error;
      apod_send_lasterror = send_timeout;
    }
  }
}


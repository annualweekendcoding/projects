/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    Webserver Applikation

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 

Mögliche Variablenkodierungen in Webseiten:

Variablenkodierungen beginnen mit %

-> Variablentags

Format Zeichen1 Zeichen2 @ Adresse [@ Format]

Zeichen1: Art der Ersetzung

V - ersetzen durch Variablenwert
O - ersetzen durch on/off 
I - ersetzen durch input-Tag (nur mit B oder I )
C - ersetzen durch Hiddenfield und Checkbox (nur mit B oder X)
//S - ersetzen durch Submit

Zeichen2:

B - Index aus var_byte_array
I - Index aus var_int_array
S - Index aus var_str_array
X - Index und Bitnummer aus var_bit_array

Adresse

Indexangabe [00..99]
Index mit Bitnummer [00..99].[0..7]

Format Default D0

D[0..9] - Ausgabe mit Anzahl Stellen 
N[0..9] - Ausgabe negativiert mit Anzahl Stellen
X[0..9] - Ausgabe Hexadezimal mit Anzahl der Stellen
F[0..9] - Ausgabe als Festpunktzahl mit Anzahl Kommastellen

-> Sondertags
%GETCOUNTER - Besucherzähler
%END - beendet die Kommunikation und damit die Webseite
-> veraltete Tags nur bei Definition von HTTP_OBSOLETE_TAGS verfügbar
%PORT[A-D][0-7] - wird bei 1 durch checked ersetzt
%PIN[A-D][0-7] - wird durch ledon.gif oder ledoff.gif ersetzt
-> noch nicht implementiert
%IF@[00..99].[0-7]....@ - Wenn Bit-Variable aus var_bit_array 1 ist wird Text bis zum nächsten @ ausgegeben
%NI@[00..99].[0-7]....@ - Wenn Bit-Variable aus var_bit_array 0 ist wird Text bis zum nächsten @ ausgegeben


Beispiele:

<img src=\"led%OX@02.1.gif\">

erzeugt einen Bildlink zu ledon.gif oder ledoff.gif in abhängigkeit vom Bit 1 in Byte 0 von var_bit_array

------------------------------------------------------------------------------*/


#include "httpd.h"
#include "webpage.h"

struct http_table http_entry[MAX_TCP_ENTRY];

//Hier wird das codierte Passwort aus config.h gespeichert.
unsigned char http_auth_passwort[20];

//----------------------------------------------------------------------------
#if MAX_VAR_BYTE_ARRAY>0
// Feld zur Aufnahme der Byte-Variablen 
uint8_t var_byte_array[MAX_VAR_BYTE_ARRAY];
#endif

#if MAX_VAR_INT_ARRAY>0
// Feld zur Aufnahme der Integer-Variablen 
int var_int_array[MAX_VAR_INT_ARRAY];
#endif

#if MAX_VAR_BIT_ARRAY>0
// Feld zur Aufnahme der Bit-Variablen 
TByte var_bit_array[MAX_VAR_BIT_ARRAY];
#endif

#if MAX_VAR_STR_ARRAY>0
// Feld zur Aufnahme der String-Variablen 
char* var_str_array[MAX_VAR_STR_ARRAY];
#endif
//----------------------------------------------------------------------------
#ifdef HTTP_GETCOUNTER
// Besuchercounter in Webpage mit %GETCOUNTER einfügen
unsigned int http_getcount = 0;
#endif

PROGMEM char http_header1[]={	"HTTP/1.0 200 Document follows\r\n"
								"Server: AVR_Small_Webserver\r\n"
								"Content-Type: text/html\r\n\r\n"};

PROGMEM char http_header2[]={	"HTTP/1.0 200 Document follows\r\n"
								"Server: AVR_Small_Webserver\r\n"
								"Content-Type: image/jpg\r\n\r\n"};

PROGMEM char http_header3[]={	"HTTP/1.0 401 Unauthorized\r\n"
								"Server: AVR_Small_Webserver\r\n"
								"WWW-Authenticate: Basic realm=\"NeedPassword\""
								"\r\nContent-Type: text/html\r\n\r\n"};

//----------------------------------------------------------------------------
#ifdef HTTP_FORMS
// Vordefinierte Strings für Steuerelemente
PROGMEM char http_input[] = "<input type=\"";
PROGMEM char http_input_text[] = "text";
PROGMEM char http_input_checkbox[] = "checkbox";
PROGMEM char http_input_hidden[] = "hidden";
PROGMEM char http_input_name[] = "\" name=\"";
PROGMEM char http_input_value[] = "\" value=\"";
PROGMEM char http_input_close[] = "\">\r\n";
#endif // HTTP_FORMS
//----------------------------------------------------------------------------
//Kein Zugriff Seite bei keinem Passwort
PROGMEM char Page0[] = {"401 Unauthorized%END"};

unsigned char rx_header_end[5] = {"\r\n\r\n\0"};

//----------------------------------------------------------------------------
//Initialisierung des Httpd Testservers
void httpd_init (void)
{
  #if MAX_VAR_STR_ARRAY>0
  // Stringpointer aus dem Variablenarray auf NULL setzen, sonst evtl. Zeigerprobleme
  for (uint8_t i=0; i<MAX_VAR_STR_ARRAY; i++) var_str_array[i]=0;
  #endif

	//HTTP_AUTH_STRING 
	decode_base64((unsigned char*)HTTP_AUTH_STRING,http_auth_passwort);

	//Serverport und Anwendung eintragen
	add_tcp_app (HTTPD_PORT, (void(*)(unsigned char))httpd);
}
   
//----------------------------------------------------------------------------
//http Testserver
void httpd (unsigned char index)
{
    //Verbindung wurde abgebaut!
    if (tcp_entry[index].status & FIN_FLAG)
    {
        return;
    }

	//Allererste Aufruf des Ports für diese Anwendung
	//HTTPD_Anwendungsstack löschen
	if(tcp_entry[index].app_status==1)
	{
		httpd_stack_clear(index);
	}
	
	//HTTP wurde bei dieser Verbindung zum ersten mal aufgerufen oder
	//HTML Header Retransmission!
	if (tcp_entry[index].app_status <= 2)
	{	
		httpd_header_check (index);
		return;
	}
	
	//Der Header wurde gesendet und mit ACK bestätigt (tcp_entry[index].app_status+1)
	//war das HTML Packet fertig, oder müssen weitere Daten gesendet werden, oder Retransmission?
	if (tcp_entry[index].app_status > 2 && tcp_entry[index].app_status < 0xFFFE)
	{
		httpd_data_send (index);
		return;
	}
	
	//Verbindung kann geschlossen werden! Alle HTML Daten wurden gesendet TCP Port kann
	//geschlossen werden (tcp_entry[index].app_status >= 0xFFFE)!!
	if (tcp_entry[index].app_status >= 0xFFFE)
	{
		tcp_entry[index].app_status = 0xFFFE;
		tcp_Port_close(index);
		return;
	}
	return;
}

//----------------------------------------------------------------------------
//HTTPD_STACK löschen
void httpd_stack_clear (unsigned char index)
{
	http_entry[index].http_header_type =0;
	http_entry[index].first_switch = 0;
	http_entry[index].http_auth = HTTP_AUTH_DEFAULT;
	http_entry[index].new_page_pointer = 0;
	http_entry[index].old_page_pointer = 0;
	http_entry[index].post = 0;
	http_entry[index].auth_ptr = http_auth_passwort;
//	http_entry[index].post_ptr = post_in;
//	http_entry[index].post_ready_ptr = post_ready;
	http_entry[index].hdr_end_pointer = rx_header_end;
	#if USE_CAM
	http_entry[index].cam = 0;
	#endif //USE_CAM				
	HTTP_DEBUG("\r\n*** NEUE HTTP ANFORDERUNG ***\r\n\r\n");	
	return;
}

//----------------------------------------------------------------------------
//Eintreffenden Header vom Client checken
void httpd_header_check (unsigned char index)
{
	unsigned int a = 0;
	
	if(strcasestr_P((char*)&eth_buffer[TCP_DATA_START_VAR],PSTR("POST"))!=0)
	{
		http_entry[index].post = 1;
	}
	
	//finden der Authorization und das Ende im Header auch über mehrere Packete hinweg!!	
	if(*http_entry[index].hdr_end_pointer != 0)
	{		
		for(a=TCP_DATA_START_VAR;a<(TCP_DATA_END_VAR);a++)
		{	
			HTTP_DEBUGPF((PSTR("%c"),eth_buffer[a]));
			
			if(!http_entry[index].http_auth) 
			{
				if (eth_buffer[a] != *http_entry[index].auth_ptr++)
				{
					http_entry[index].auth_ptr = http_auth_passwort;
				}
				if(*http_entry[index].auth_ptr == 0) 
				{
					http_entry[index].http_auth = 1;
					HTTP_DEBUG("  <---LOGIN OK!--->\r\n");
				}
			}
			
			if (eth_buffer[a] != *http_entry[index].hdr_end_pointer++)
			{
				http_entry[index].hdr_end_pointer = rx_header_end;
			}
			
			//Das Headerende wird mit (CR+LF+CR+LF) angezeigt!
			if(*http_entry[index].hdr_end_pointer == 0) 
			{
				HTTP_DEBUG("<---HEADER ENDE !--->\r\n");
				break;
			}
		}
	}
	
	//Einzelne Postpacket (z.B. bei firefox)
  #ifdef HTTP_FORMS
	if(http_entry[index].http_auth && http_entry[index].post == 1)
	{
		HTTP_DEBUG("\r\n<--- POST auswerten --->");
    uint8_t state=0;
    // Das funktioniert nur, wenn der POST für ein Element im selben Paket liegt
    // In ungünstigen Fällen können auch falsche Werte übernommen werden, 
    // wenn z.B. eine Integerzahl über die Paketgrenze aufgeteilt wird
		for(a = TCP_DATA_START_VAR;a<(TCP_DATA_END_VAR);a++)
		{	
			HTTP_DEBUGPF((PSTR("%c"),eth_buffer[a]));
      if (state==0)  
      {
			  if (strncasecmp_P((char*)eth_buffer+a,PSTR("SUB="),4)==0)
        {
				  HTTP_DEBUG("\r\nPOST SUB gefunden");
          // Ende des Formulars POST nicht weiter auswerten
          http_entry[index].post = 0;
          break;
        }
        else
        {
          // Das sollte ein Tagname sein
          // Ob es tatsächlich einer ist kommt später heraus
          char firstchar = eth_buffer[a];
          char secondchar = eth_buffer[a+1];
          // Der Feldindex ist 2-stellig
 				  uint8_t b = (eth_buffer[a+3]-48)*10;
			    b += (eth_buffer[a+4]-48);
          a += 5;
          uint8_t bit = 255;
          if (eth_buffer[a]=='.')
          {
            bit = eth_buffer[a+1]-48;
            a += 2;
          }
          if (eth_buffer[a]=='=')
          {
            state = 1;
            // a steht jetzt auf dem =
            // suchen nach Input 
	  			  if (firstchar=='I')
            {
	  				  HTTP_DEBUG("\r\nPOST I gefunden");
              // den Wert nach dem = in Int wandeln
              int value = atoi((char*)eth_buffer+a+1);
              #if MAX_VAR_INT_ARRAY>0
              if (secondchar=='I' && b<MAX_VAR_INT_ARRAY)
              {
                var_int_array[b] = value;
                HTTP_DEBUG("Int - fehlerfrei");
              }
              else
              #endif
              #if MAX_VAR_BYTE_ARRAY>0
              if (secondchar=='B' && b<MAX_VAR_BYTE_ARRAY)
              {
                var_byte_array[b] = value;
                HTTP_DEBUG("Byte - fehlerfrei");
              }
              #endif
              ;
            }
            // suchen nach Checkbox 
		  		  else if (firstchar=='C')
            {
					    HTTP_DEBUG("\r\nPOST C gefunden");
              #if MAX_VAR_BIT_ARRAY>0
              if (secondchar=='X' && b<MAX_VAR_BIT_ARRAY && bit<8)
              {
                // wenn Wert nach dem = 1 ist dann true sonst false schreiben
                if (eth_buffer[a+1]=='1') var_bit_array[b].byte |= 1<<bit;
                else var_bit_array[b].byte &= ~ (1<<bit);
                HTTP_DEBUG(" - fehlerfrei");
              }
              else
              #endif
              #if MAX_VAR_BYTE_ARRAY>0
              if (secondchar=='B' && b<MAX_VAR_BYTE_ARRAY && bit<8)
              {
                // wenn Wert nach dem = 1 ist dann true sonst false schreiben
                if (eth_buffer[a+1]=='1') var_byte_array[b] |= 1<<bit;
                else var_byte_array[b] &= ~ (1<<bit);
                HTTP_DEBUG(" - fehlerfrei");
              }
              #endif
              ;
            }
          }   
        }
        #if USE_WOL
        if (eth_buffer[a+1]=='W') wol_enable = 1;
        #endif //USE_WOL
			}
	    else 
      {
        // nach & kommt evtl. wieder eine Variablenbezeichnung
        if (eth_buffer[a]=='&') state=0;
      }	
		}
		HTTP_DEBUG("\r\n<--- POST auswerten Ende --->");
	}	
  #endif // HTTP_FORMS
	
	//Welche datei wird angefordert? Wird diese in der Flashspeichertabelle gefunden?
	unsigned char page_index = 0;
	
	if (!http_entry[index].new_page_pointer)
	{
		for(a = TCP_DATA_START_VAR+5;a<(TCP_DATA_END_VAR);a++)
		{
			if (eth_buffer[a] == '\r')
			{
				eth_buffer[a] = '\0';
				break;
			}
		}
	
		while(WEBPAGE_TABLE[page_index].filename)
		{
			if (strcasestr((char*)&eth_buffer[TCP_DATA_START_VAR],WEBPAGE_TABLE[page_index].filename)!=0) 
				{
					http_entry[index].http_header_type = 1;
					HTTP_DEBUG("\r\n\r\nDatei gefunden: ");
					HTTP_DEBUGPF((PSTR("%s"),(char*)WEBPAGE_TABLE[page_index].filename));
					HTTP_DEBUG("<----\r\n\r\n");	
					if (strcasestr(WEBPAGE_TABLE[page_index].filename,".jpg")!=0)
					{
						#if USE_CAM
						if (strcasestr(WEBPAGE_TABLE[page_index].filename,"camera")!=0)
						{	
							http_entry[index].cam = 1;
						}
						#endif //USE_CAM
						http_entry[index].http_header_type = 1;
					}
					if (strcasestr(WEBPAGE_TABLE[page_index].filename,".gif")!=0)
					{
						http_entry[index].http_header_type = 1;
					}	
					if (strcasestr(WEBPAGE_TABLE[page_index].filename,".htm")!=0)
					{
						http_entry[index].http_header_type = 0;	
					}	
					http_entry[index].new_page_pointer = WEBPAGE_TABLE[page_index].page_pointer;
					break;
				}
			page_index++;
		}
	}

	//Wurde das Ende vom Header nicht erreicht
	//kommen noch weitere Stücke vom Header!
	if ((*http_entry[index].hdr_end_pointer != 0) || (http_entry[index].post == 1))
	{
		//Der Empfang wird Quittiert und es wird auf weiteres Headerstück gewartet
		tcp_entry[index].status =  ACK_FLAG;
		create_new_tcp_packet(0,index);
		//Warten auf weitere Headerpackete
		tcp_entry[index].app_status = 1;
		return;
	}	
	
	//Wurde das Passwort in den ganzen Headerpacketen gefunden?
	//Wenn nicht dann ausführen und Passwort anfordern!
	if((!http_entry[index].http_auth) && tcp_entry[index].status&PSH_FLAG)
	{	
		//HTTP_AUTH_Header senden!
		http_entry[index].new_page_pointer = Page0;
		memcpy_P((char*)&eth_buffer[TCP_DATA_START_VAR],http_header3,(sizeof(http_header3)-1));
		tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
		create_new_tcp_packet((sizeof(http_header3)-1),index);
		tcp_entry[index].app_status = 2;
		return;
	}
	
	//Standart INDEX.HTM Seite wenn keine andere gefunden wurde
	if (!http_entry[index].new_page_pointer)
	{
    #ifdef HTTP_GETCOUNTER
		//Besucher Counter
		http_getcount++;
		#endif
		http_entry[index].new_page_pointer = PageIndex;
		http_entry[index].http_header_type = 0;
	}	
	
	tcp_entry[index].app_status = 2;
	//Seiten Header wird gesendet
	if(http_entry[index].http_header_type == 1)
	{
		memcpy_P((char*)&eth_buffer[TCP_DATA_START_VAR],http_header2,(sizeof(http_header2)-1));
        tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
        create_new_tcp_packet((sizeof(http_header2)-1),index);
        return;
	}
     
	if(http_entry[index].http_header_type == 0)
	{
		memcpy_P((char*)&eth_buffer[TCP_DATA_START_VAR],http_header1,(sizeof(http_header1)-1));
        tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
        create_new_tcp_packet((sizeof(http_header1)-1),index);
        return;
	}
    return;
}

//----------------------------------------------------------------------------
//Daten Packete an Client schicken
void httpd_data_send (unsigned char index)
{	
	unsigned int a;
	char var_conversion_buffer[CONVERSION_BUFFER_LEN];
	
	//Passwort wurde im Header nicht gefunden
	if(!http_entry[index].http_auth)
	{
		http_entry[index].new_page_pointer = Page0;
		#if USE_CAM
		http_entry[index].cam = 0;
		#endif //USE_CAM
	}
	
	#if USE_CAM //*****************************************************************
	unsigned long byte_counter = 0;
	
	if(http_entry[index].cam > 0)
	{
        //Neues Bild wird in den Speicher der Kamera geladen!
		if(http_entry[index].cam == 1){
			max_bytes = cam_picture_store(CAM_RESELUTION);
			http_entry[index].cam = 2;
		}
        
		for (a = 0;a < (MTU_SIZE-(TCP_DATA_START)-10);a++)
		{
			byte_counter = ((tcp_entry[index].app_status - 3)*(MTU_SIZE-(TCP_DATA_START)-10)) + a;
			
			eth_buffer[TCP_DATA_START + a] = cam_data_get(byte_counter);
			
			if(byte_counter > max_bytes)
			{
				tcp_entry[index].app_status = 0xFFFD;
				a++;
				break;
			}
		}
		//Erzeugte Packet kann nun gesendet werden!
		tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
		create_new_tcp_packet(a,index);
		return;	
	}
	#endif //USE_CAM***************************************************************

	//kein Packet empfangen Retransmission des alten Packetes
	if (tcp_entry[index].status == 0) 
	{
		http_entry[index].new_page_pointer = http_entry[index].old_page_pointer;
	}
	http_entry[index].old_page_pointer = http_entry[index].new_page_pointer;

  // TODO: Problem, da Längenüberschreitung durch Ersetzen von Tags auftreten kann.
  // möglicherweise dienen die -150 dazu die Grenze etwas offen zu lassen
	for (a = 0;a<(MTU_SIZE-(TCP_DATA_START)-150);a++)
	{
		unsigned char b;
		b = pgm_read_byte(http_entry[index].new_page_pointer++);
		eth_buffer[TCP_DATA_START + a] = b;
		// Müssen Variablen ins Packet eingesetzt werden? 
    // Beschreibung in webpage.h
		if (b == '%')
		{
			if (strncasecmp_P("END",http_entry[index].new_page_pointer,3)==0)
			{	
        //wurde das Ende des Packetes erreicht?
	  		//Verbindung TCP Port kann beim nächsten ACK geschlossen werden
		  	//Schleife wird abgebrochen keine Daten mehr!!
				tcp_entry[index].app_status = 0xFFFD;
				break;
			}
			#ifdef HTTP_OBSOLETE_TAGS
			//Einsetzen des Pin Status %PINxy durch "ledon" oder "ledoff"
			//x = 0 : PINA / x = 1 : PINB / x = 2 : PINC / x = 3 : PIND
      else if (strncasecmp_P("PIN",http_entry[index].new_page_pointer,3)==0)
			{
				unsigned char pin  = (pgm_read_byte(http_entry[index].new_page_pointer+4)-48);	
				b = 0;
				switch(pgm_read_byte(http_entry[index].new_page_pointer+3))
				{
					case 'A':
						b = (PINA & (1<<pin));
						break;
					case 'B':
						b = (PINB & (1<<pin));
						break;
					case 'C':
						b = (PINC & (1<<pin));
						break;
					case 'D':
						b = (PIND & (1<<pin));
						break;    
				}				
				if(b)
				{
					strcpy_P(var_conversion_buffer, PSTR("ledon.gif"));
				}
				else
				{
					strcpy_P(var_conversion_buffer, PSTR("ledoff.gif"));
				}
				uint8_t str_len = strnlen(var_conversion_buffer,CONVERSION_BUFFER_LEN);
				memmove(&eth_buffer[TCP_DATA_START+a],var_conversion_buffer,str_len);
				a += str_len-1;
				http_entry[index].new_page_pointer += 5;
			}
      #ifdef HTTP_FORMS
			//Einsetzen des Port Status %PORTxy durch "checked" wenn Portx.Piny = 1
			//x: A..G  y: 0..7 
			if (strncasecmp_P("PORT",http_entry[index].new_page_pointer,4)==0)
			{
				unsigned char pin  = (pgm_read_byte(http_entry[index].new_page_pointer+5)-48);	
				b = 0;
				switch(pgm_read_byte(http_entry[index].new_page_pointer+4))
				{
					case 'A':
						b = (PORTA & (1<<pin));
						break;
					case 'B':
						b = (PORTB & (1<<pin));
						break;
					case 'C':
						b = (PORTC & (1<<pin));
						break;
					case 'D':
						b = (PORTD & (1<<pin));
						break; 
				}
				
				if(b)
				{
					strcpy_P(var_conversion_buffer, PSTR("checked"));
				}
				else
				{
					strcpy_P(var_conversion_buffer, PSTR("\0"));
				}
				uint8_t str_len = strnlen(var_conversion_buffer,CONVERSION_BUFFER_LEN);
				memmove(&eth_buffer[TCP_DATA_START+a],var_conversion_buffer,str_len);
				a += str_len-1;
				http_entry[index].new_page_pointer += 6;
			}
			#endif // HTTP_FORMS
			#endif // HTTP_OBSOLETE_TAGS
      #ifdef HTTP_GETCOUNTER
      // ersetzen von %GETCOUNTER durch Besucherzähler
			else if (strncasecmp_P("GETCOUNTER",http_entry[index].new_page_pointer,10)==0)
			{	
				itoa (http_getcount,var_conversion_buffer,10);
				uint8_t str_len = strnlen(var_conversion_buffer,CONVERSION_BUFFER_LEN);
				memmove(&eth_buffer[TCP_DATA_START+a],var_conversion_buffer,str_len);
				a = a + (str_len-1);  
				http_entry[index].new_page_pointer += 10;
			}
			#endif
      else if (pgm_read_byte(http_entry[index].new_page_pointer+2)=='@')
      {
        // Es ist ein Variablenformat
        // TODO: Zahlenangaben mit variabler Stellenzahl zulassen
        // TODO: Groß/Kleinschreibung missachten
        // Zeiger auf Anfang merken, wird bei FORMS gebraucht
        PGM_P page_pointer = http_entry[index].new_page_pointer;
        char firstchar = pgm_read_byte(page_pointer);
        char secondchar = pgm_read_byte(page_pointer+1);
        // Index 2-stellig ermitteln 
				b = (pgm_read_byte(page_pointer+3)-48)*10;
				b +=(pgm_read_byte(page_pointer+4)-48);
        http_entry[index].new_page_pointer += 5;
        // evtl. Bitnummer ermitteln	
        uint8_t bit = 255;
        if (pgm_read_byte(http_entry[index].new_page_pointer)=='.')
        {
          bit = pgm_read_byte(http_entry[index].new_page_pointer+1)-48;
          http_entry[index].new_page_pointer += 2;
        }
        // evtl. Format ermitteln	
        char format = 'D'; // Standardformat Dezimal
        uint8_t digits = 0;
        if (pgm_read_byte(http_entry[index].new_page_pointer)=='@')
        {
          format = pgm_read_byte(http_entry[index].new_page_pointer+1);
          digits = pgm_read_byte(http_entry[index].new_page_pointer+2)-48;
          http_entry[index].new_page_pointer += 3;
        }
        int value = 0;
        #if MAX_VAR_BIT_ARRAY>0
        if (secondchar=='X' && b<MAX_VAR_BIT_ARRAY) value = var_bit_array[b].byte;
        #endif
        #if MAX_VAR_BYTE_ARRAY>0
        if (secondchar=='B' && b<MAX_VAR_BYTE_ARRAY) value = var_byte_array[b];
        #endif
        #if MAX_VAR_INT_ARRAY>0
        if (secondchar=='I' && b<MAX_VAR_INT_ARRAY) value = var_int_array[b];
        #endif
        // Wenn Bit angegeben dann nur das bit also 0/1 in value merken
        if (bit<16) value = (value & (1<<bit))>>bit;
			  //Einsetzen des Bit Status %O durch "on" oder "off"
			  if (firstchar=='O')
			  {
					if (value)
				  {
					  strcpy_P((char*)&eth_buffer[TCP_DATA_START+a], PSTR("on"));
            a++;
				  }
				  else
				  {
					  strcpy_P((char*)&eth_buffer[TCP_DATA_START+a], PSTR("off"));
            a+=2;
				  }
        }
        #ifdef HTTP_FORMS
        // Checkbox generieren
		  	else if (firstchar=='C')
			  {	
          // Zuerst ein hidden-field, da die Checkbox bei unchecked nicht im POST übermittelt wird
          // Im POST wird dann zuerst das hidden-field übertragen, welches den Wert 0 enthält
          // Danach kommt eventuell mit dem gleichen Namen der Wert der Checkbox = 1
          // Es wird also in ungünstigen Fällen die Variable kurz 0 gesetzt und dann auf 1, falls eine
          // Paketgrenze dazwischen lag, oder die Variable in einer ISR ausgewertet wird.
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input);
          a += strlen_P(http_input);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_hidden);
          a += strlen_P(http_input_hidden);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_name);
          a += strlen_P(http_input_name);
          // eigenen Tag-Namen kopieren, dabei @ durch _ tauschen
          for (PGM_P p = page_pointer; p<http_entry[index].new_page_pointer; p++)
          {
    				uint8_t c = pgm_read_byte(p);
            if (c=='@') c='_';
            eth_buffer[TCP_DATA_START+a++] = c;
          }
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_value);
          a += strlen_P(http_input_value);
					// Value 0 in das hidden-Field schreiben
          eth_buffer[TCP_DATA_START+a++] = '0';
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_close);
          a += strlen_P(http_input_close);
          // Jetzt die Checkbox generieren
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input);
          a += strlen_P(http_input);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_checkbox);
          a += strlen_P(http_input_checkbox);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_name);
          a += strlen_P(http_input_name);
          // eigenen Tag-Namen kopieren, dabei @ durch _ tauschen
          for (PGM_P p = page_pointer; p<http_entry[index].new_page_pointer; p++)
          {
    				uint8_t c = pgm_read_byte(p);
            if (c=='@') c='_';
            eth_buffer[TCP_DATA_START+a++] = c;
          }
          // Bei true checked einfügen
          if (value)
				  {
					  strcpy_P((char *)&eth_buffer[TCP_DATA_START+a], PSTR("\" checked=\"checked"));
            a += 18;
				  }
          // Als value 1 einfügen
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_value);
          a += strlen_P(http_input_value);
          eth_buffer[TCP_DATA_START+a++] = '1';	
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_close);
          a += strlen_P(http_input_close)-1;
        }
        else if (firstchar=='I')
        {
          // INPUT generieren
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input);
          a += strlen_P(http_input);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_text);
          a += strlen_P(http_input_text);
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_name);
          a += strlen_P(http_input_name);
          // eigenen Tag-Namen kopieren, dabei @ durch _ tauschen
          for (PGM_P p = page_pointer; p<http_entry[index].new_page_pointer; p++)
          {
    				uint8_t c = pgm_read_byte(p);
            if (c=='@') c='_';
            eth_buffer[TCP_DATA_START+a++] = c;
          }
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_value);
          a += strlen_P(http_input_value);
				  itoa (value,var_conversion_buffer,10);
				  uint8_t str_len = strnlen(var_conversion_buffer,CONVERSION_BUFFER_LEN);
				  memmove(&eth_buffer[TCP_DATA_START+a],var_conversion_buffer,str_len);
				  a += str_len;
          strcpy_P((char *)&eth_buffer[TCP_DATA_START+a],http_input_close);
          a += strlen_P(http_input_close)-1;
        }   
        #endif // HTTP_FORMS
        #if MAX_VAR_STR_ARRAY>0
        // ersetzen von %VS@00 bis %VS@99 durch Stringwert aus var_str_array
			  else if (firstchar=='V' && secondchar=='S')
			  {	
          if (b<MAX_VAR_STR_ARRAY)
          {
            char *s =	var_str_array[b];
				    if (s)
            {
              str_len = strlen(s);
  			      memmove(&eth_buffer[TCP_DATA_START+a],s,str_len);
				    }
            else
            {
              // Bei Nullzeiger "NULL" einsetzen
              str_len = 4;
				      strcpy_P(&eth_buffer[TCP_DATA_START+a],PSTR("NULL"));
            }
		  	    a = a + (str_len-1);
          }     
  			}
  			#endif // MAX_VAR_STR_ARRAY>0
        // ersetzen von %V durch Integerwert
			  else if (firstchar=='V')
        {
				  switch (format)
          {
            case 'X' : itoa (value,var_conversion_buffer,16); break;
            default  : itoa (abs(value),var_conversion_buffer,10);
          } 
          uint8_t str_len = strnlen(var_conversion_buffer,CONVERSION_BUFFER_LEN);
          if ((value<0 && format!='X') || (value>0 && format=='N'))
          {
            // Vorzeichen davorsetzen wenn nicht hex
            // oder wenn positiv und negativiert
            eth_buffer[TCP_DATA_START+a++] = '-';
          }
				  if (format=='F')
          {
            // Bei Format F wird die Stellenanzahl rechts vom Komma ausgegeben
              uint8_t i=0;
              if (str_len>digits)
              {
                // Zeichen vor dem Komma ausgeben
                for (; i<(str_len-digits); i++) eth_buffer[TCP_DATA_START+a++] = var_conversion_buffer[i];
              }
              // sonst mindestens eine 0 vor dem Komma
              else eth_buffer[TCP_DATA_START+a++] = '0';
              // Komma schreiben
              eth_buffer[TCP_DATA_START+a++] = '.';
              // noch Nullen schreiben
              for (; digits>str_len; digits--) eth_buffer[TCP_DATA_START+a++] = '0';
              // Rest des Strings ausgeben
              for (; i<str_len; i++) eth_buffer[TCP_DATA_START+a++] = var_conversion_buffer[i];
              a--;
          }
          else
          {
            // Sonst wird die Stellenanzahl links mit Nullen aufgefüllt
            for (uint8_t i=str_len; i<digits; i++) eth_buffer[TCP_DATA_START+a++] = '0';
  				  memmove(&eth_buffer[TCP_DATA_START+a],var_conversion_buffer,str_len);
				    a = a + (str_len-1);
          }
        }
      }
		}
	}
	//Erzeugte Packet kann nun gesendet werden!
	tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
	create_new_tcp_packet(a,index);
	return;
}



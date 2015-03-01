/* Konfiguration des Modbus-Slave */
/* Diese Datei als usart_raw_config.h in das Projekt aufnehmen */

// Puffergröße für Empfangspuffer
// 0 = keinen Puffer verwenden sondern direkt empfangen
#define USART_RX_BUFFER_SIZE	250

// Puffergröße für Sendepuffer
// 0 = keinen Puffer verwenden sondern direkt senden
#define USART_TX_BUFFER_SIZE	250

// Zeitmessung
// Wenn definiert, dann die Zeit des letzen Empfangs merken
#define USART_SAVE_LASTTIME

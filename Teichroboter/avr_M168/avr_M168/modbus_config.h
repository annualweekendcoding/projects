/* Konfiguration des Modbus-Slave */
/* Diese Datei als modbus_config.h in das Projekt aufnehmen */

/* Slave ID we respond to */
#define MODBUS_SLAVE_ID	11

// Max amounts of BitAddresses the modbus master can come up with
// Das sollte ein Vielfaches von 8 sein
#define MODBUS_MAX_BITADDRESSES		32

// Max amounts of WordAddresses the modbus master can come up with
#define MODBUS_MAX_WORDADDRESSES	20








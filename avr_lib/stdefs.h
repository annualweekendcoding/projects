#ifndef STDEFS
#define STDEFS

/* Datentypen */
#define BOOL uint8_t
#define BYTE uint8_t
#define WORD uint16_t
#define SINT int8_t
#define INT int16_t
#define UINT uint16_t
#define DINT int32_t
#define UDINT uint32_t
#define LINT int64_t
#define ULINT uint64_t
#define REAL float
#define LREAL double
#define VOID void
#define TIME DINT

/* Konvertierfunktionen */
#define BOOL_TO_BYTE
#define BOOL_TO_WORD
#define BOOL_TO_DWORD
#define BYTE_TO_BOOL
#define BYTE_TO_WORD
#define BYTE_TO_DWORD
#define WORD_TO_BOOL
#define WORD_TO_BYTE
#define WORD_TO_DWORD
#define WORD_TO_INT
#define DWORD_TO_BOOL
#define DWORD_TO_BYTE
#define DWORD_TO_WORD
#define DWORD_TO_DINT
#define INT_TO_WORD
#define INT_TO_DINT
#define INT_TO_REAL (float)
#define INT_TO_STRING
#define DINT_TO_DWORD
#define DINT_TO_INT
#define DINT_TO_REAL (float)
#define DINT_TO_STRING
#define DINT_TO_TIME
#define REAL_TO_INT
#define REAL_TO_DINT
#define REAL_TO_STRING
#define STRING_TO_INT
#define STRING_TO_DINT
#define STRING_TO_REAL
#define TIME_TO_DINT

/* Arithmetische Funktionen */
#define ABS
#define SQR
#define SQRT

/* Logarithmische Funktionen */
#define EXP
#define LN
#define LOG

/* Trigonometrische Funktionen */
#define SIN
#define COS
#define TAN
#define ASIN
#define ACOS
#define ATAN

/* Bitkettenfunktionen */
#define ROL
#define ROR
#define SHL
#define SHR

/* Zeichenkettenfunktionen */
#define LEN
#define LEFT
#define RIGHT
#define MID
#define CONCAT
#define INSERT
#define DELETE
#define REPLACE
#define FIND
#define EQ_STRING
#define NE_STRING

/* Funktionen für Zeitglieder */
#define SETIME

/* Funktionen für Flankenerkennung */
#define RTRIG
#define FTRIG

/* Konstanten */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif

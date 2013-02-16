#ifndef _MEMCHECK_H_
#define _MEMCHECK_H_

/********************************

kopier von http://www.rn-wissen.de/index.php/Speicherverbrauch_bestimmen_mit_avr-gcc

Mit der folgenden kleinen Routine kann der noch freie SRAM-Bereich bestimmt werden. 
Es wird nicht der momentan freie Speicher bestimmt, sondern das Minimum an Speicher, 
das bis dato frei geblieben ist. 
Dazu wird im Startup-Code das Muster 0xaa in den SRAM geschrieben. 
Durch Aufruf der Funktion get_mem_unused wird bestimmt, wieviel von diesem Muster noch intakt ist. 
Dieses Vorgehen ist deshalb notwendig, weil es auch ISR-Routinen gibt, 
die dynamisch Splatz (auf dem Stack) brauchen, 
und man eine Routine zur Bestimmung des Speicherverbrauchs nicht in den ISRs aufrufen will.

Mit optimierendem Compiler brauchen die beiden Routinen 42 Bytes an Flash.

Damit der Code den richtigen Wert liefert, darf keine dynamische Speicherallokierung mit malloc() etc. geschehen sein; 
ein __builtin_alloca ist hingegen kein Problem, da letzteres den Platz vom Stapel nimmt.

Die Funktion init_mem wird in der Init-Phase vor main aufgerufen.
*********************************/

extern unsigned short get_mem_unused (void);

#endif  /* _MEM_CHECK_H_ */

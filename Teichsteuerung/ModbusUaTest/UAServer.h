//---------------------------------------------------------------------------

#ifndef UAServerH
#define UAServerH
//---------------------------------------------------------------------------
void UAServer_start();
void UAServer_loop();

void UAServer_addIntVariable(const char* nodeId, const char* browseName, const char* displayName, const char* description, int initValue, int bitsize);
void UA_Server_writeIntVariable(const char* nodeId, int value);
void UA_Server_setVariableBad(const char* nodeId);

#endif

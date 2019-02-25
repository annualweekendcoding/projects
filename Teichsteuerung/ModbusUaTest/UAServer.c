//---------------------------------------------------------------------------

#pragma hdrstop

#include "UAServer.h"
//---------------------------------------------------------------------------
#include "open62541.h"

UA_ServerConfig *ua_config = NULL;
UA_Server *ua_server = NULL;
bool ua_running = false;

int *__access=NULL;

void UAServer_start()
{
  if (!ua_config) ua_config = UA_ServerConfig_new_default();
  if (!ua_server) ua_server = UA_Server_new(ua_config);

  if (!ua_running) {
    UA_StatusCode status = UA_Server_run_startup(ua_server);
    ua_running=true;
  }
  else
  {
    UA_StatusCode status = UA_Server_run_shutdown(ua_server);
    ua_running=false;
  }
  //  UA_Server_delete(server);
  //  UA_ServerConfig_delete(config);
}

void UAServer_loop()
{
  if (ua_running) {
    UA_UInt16 result = UA_Server_run_iterate(ua_server,true);
  }
}

void UAServer_addIntVariable(const char* nodeId, const char* browseName, const char* displayName, const char* description, int initValue, int bitsize) {
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Int32 myInteger = initValue;
    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US",(char*)description);
    attr.displayName = UA_LOCALIZEDTEXT("en-US",(char*)displayName);
    if (bitsize==16) attr.dataType = UA_TYPES[UA_TYPES_INT16].typeId;
    else if (bitsize==32) attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    else attr.dataType = UA_TYPES[UA_TYPES_INT64].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, (char*)nodeId);
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, (char*)browseName);
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(ua_server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
}

/**
 * Now we change the value with the write service. This uses the same service
 * implementation that can also be reached over the network by an OPC UA client.
 */

void UA_Server_writeIntVariable(const char* nodeId, int value) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, (char*)nodeId);

    /* Write a different integer value */
    UA_Int32 myInteger = value;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(ua_server, myIntegerNodeId, myVar);
}

void UA_Server_setVariableBad(const char* nodeId) {
  UA_NodeId myNodeId = UA_NODEID_STRING(1, (char*)nodeId);

  /* Set the status code of the value to an error code. The function
   * UA_Server_write provides access to the raw service. The above
   * UA_Server_writeValue is syntactic sugar for writing a specific node
   * attribute with the write service. */
  UA_WriteValue wv;
  UA_WriteValue_init(&wv);
  wv.nodeId = myNodeId;
  wv.attributeId = UA_ATTRIBUTEID_VALUE;
  wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
  wv.value.hasStatus = true;
  UA_Server_write(ua_server, &wv);
}

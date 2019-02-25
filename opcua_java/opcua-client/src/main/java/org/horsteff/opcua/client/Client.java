package org.horsteff.opcua.client;

import java.util.Arrays;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import org.eclipse.milo.opcua.sdk.client.OpcUaClient;
import org.eclipse.milo.opcua.sdk.client.api.config.OpcUaClientConfig;
import org.eclipse.milo.opcua.sdk.client.api.identity.AnonymousProvider;
import org.eclipse.milo.opcua.sdk.client.api.nodes.VariableNode;
import org.eclipse.milo.opcua.sdk.client.model.nodes.objects.ServerNode;
import org.eclipse.milo.opcua.stack.client.UaTcpStackClient;
import org.eclipse.milo.opcua.stack.core.Identifiers;
import org.eclipse.milo.opcua.stack.core.Stack;
import org.eclipse.milo.opcua.stack.core.UaException;
import org.eclipse.milo.opcua.stack.core.security.SecurityPolicy;
import org.eclipse.milo.opcua.stack.core.types.builtin.DataValue;
import org.eclipse.milo.opcua.stack.core.types.builtin.LocalizedText;
import org.eclipse.milo.opcua.stack.core.types.builtin.NodeId;
import org.eclipse.milo.opcua.stack.core.types.builtin.StatusCode;
import org.eclipse.milo.opcua.stack.core.types.builtin.Variant;
import org.eclipse.milo.opcua.stack.core.types.structured.CallMethodRequest;
import org.eclipse.milo.opcua.stack.core.types.structured.EndpointDescription;
import org.eclipse.milo.opcua.stack.core.types.structured.ServerStatusDataType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static org.eclipse.milo.opcua.stack.core.types.builtin.unsigned.Unsigned.uint;

@SuppressWarnings("Duplicates")
public class Client
{
    private final Logger logger = LoggerFactory.getLogger(getClass());
    private static final String ENDPOINT_URL = "opc.tcp://localhost:12686/example";

    public static void main(String[] args)
    {
        final Client client = new Client();
        client.run();
    }

    private void run()
    {
        final OpcUaClient client;
        try {
            EndpointDescription[] endpointDescriptions;
            try {
                endpointDescriptions = UaTcpStackClient.getEndpoints(ENDPOINT_URL).get();
            }
            catch (InterruptedException | ExecutionException e) {
                logger.warn("Connection failed: {}", e.getMessage(), e);
                final String discoveryUrl =  ENDPOINT_URL + "/discovery";
                logger.info("Trying discovery URL: {}", discoveryUrl);
                endpointDescriptions = UaTcpStackClient.getEndpoints(discoveryUrl).get();
            }

            final EndpointDescription endpoint = Arrays.stream(endpointDescriptions).filter(
                    endpoint1 -> endpoint1.getSecurityPolicyUri().equals(
                            SecurityPolicy.None.getSecurityPolicyUri())).findFirst().orElseThrow(
                    () -> new RuntimeException("No desired endpoints found"));

            final OpcUaClientConfig config = OpcUaClientConfig.builder()
                    .setApplicationName(LocalizedText.english("My OPC UA Example Client"))
                    .setApplicationUri("urn:horsteff:opcua:example:client")
                    .setEndpoint(endpoint)
                    .setIdentityProvider(new AnonymousProvider())
                    .setRequestTimeout(uint(5000))
                    .build();

            client = new OpcUaClient(config);

            try {
                client.connect().get();

                final ServerNode serverNode = client.getAddressSpace().getObjectNode(Identifiers.Server,
                        ServerNode.class).get();
                final String[] serverArray = serverNode.getServerArray().get();
                final String[] namespaceArray = serverNode.getNamespaceArray().get();

                logger.info("ServerArray={}", Arrays.toString(serverArray));
                logger.info("NamespaceArray={}", Arrays.toString(namespaceArray));

                // Read the value of attribute the ServerStatus variable component
                ServerStatusDataType serverStatus = serverNode.getServerStatus().get();

                logger.info("ServerStatus={}", serverStatus);

                for (int i = 0; i < namespaceArray.length; i++) {
                    String s = namespaceArray[i];
                    if (s.equals("urn:horsteff:opcua:example")) {
                        final NodeId nodeId = new NodeId(i, "HelloWorld/aValue");

                        final VariableNode variableNode = client.getAddressSpace().createVariableNode(
                                nodeId);
                        final DataValue dataValue = variableNode.readValue().get();
                        final Variant variant = dataValue.getValue();
                        final Object value = variant.getValue();
                        final NodeId typeId = variableNode.getDataType().get();
                        logger.info("aValue={}, type={}, class={}", value, typeId, value.getClass().getName());

                        if (Identifiers.Int32.equals(typeId)) {
                            logger.info("Write {} to node");
                            final int newvalue = ((Integer)value) + 1;
                            final CompletableFuture<StatusCode> future = client.writeValue(
                                    nodeId, new DataValue(new Variant(newvalue)));
                            final StatusCode statusCode = future.get();
                            if (statusCode.isGood()) {
                                logger.info("Wrote {} to nodeId {}", newvalue, nodeId);
                            } else {
                                logger.info("Could not write {} to nodeId {}, status={}", newvalue, nodeId, statusCode);
                            }
                        }

                        final CompletableFuture<Void> future = new CompletableFuture<>();

                        myMethod(client, i, "Hi Server!").exceptionally(throwable -> {
                            logger.error("Error invoking myMethod: {}", throwable.getMessage(), throwable);
                            return "";
                        }).thenAccept(s1 -> {
                            logger.info("myMethod returned: {}", s1);
                            future.complete(null);
                        });

                        future.get();
                    }
                }

            }
            catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

            try {
                client.disconnect().get();
                Stack.releaseSharedResources();
            }
            catch (InterruptedException | ExecutionException e) {
                logger.error("Error disconnecting: {}", e.getMessage(), e);
            }
        }
        catch (InterruptedException | ExecutionException e) {
            logger.error("Error running code: {}", e.getMessage(), e);
        }
    }

    @SuppressWarnings("SameParameterValue")
    private CompletableFuture<String> myMethod(OpcUaClient client, final int namespaceIndex, final String s)
    {
        final NodeId objectId = new NodeId(namespaceIndex, "HelloWorld");
        final NodeId methodId = new NodeId(namespaceIndex, "HelloWorld/myMethod");
        final CallMethodRequest request = new CallMethodRequest(objectId, methodId, new Variant[]{new Variant(s)});

        return client.call(request).thenCompose(result -> {
            final StatusCode statusCode = result.getStatusCode();
            if (statusCode.isGood()) {
                @SuppressWarnings("ConstantConditions")
                final String outS = (String)result.getOutputArguments()[0].getValue();
                return CompletableFuture.completedFuture(outS);
            } else {
                final CompletableFuture<String> future = new CompletableFuture<>();
                future.completeExceptionally(new UaException(statusCode));
                return future;
            }
        });
    }
}

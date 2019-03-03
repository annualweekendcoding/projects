package org.horsteff.opcua.client;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import org.eclipse.milo.opcua.sdk.client.OpcUaClient;
import org.eclipse.milo.opcua.sdk.client.api.config.OpcUaClientConfig;
import org.eclipse.milo.opcua.sdk.client.api.identity.AnonymousProvider;
import org.eclipse.milo.opcua.sdk.client.api.subscriptions.UaMonitoredItem;
import org.eclipse.milo.opcua.sdk.client.api.subscriptions.UaSubscription;
import org.eclipse.milo.opcua.sdk.client.model.nodes.objects.ServerNode;
import org.eclipse.milo.opcua.stack.client.UaTcpStackClient;
import org.eclipse.milo.opcua.stack.core.AttributeId;
import org.eclipse.milo.opcua.stack.core.Identifiers;
import org.eclipse.milo.opcua.stack.core.Stack;
import org.eclipse.milo.opcua.stack.core.security.SecurityPolicy;
import org.eclipse.milo.opcua.stack.core.types.builtin.DataValue;
import org.eclipse.milo.opcua.stack.core.types.builtin.LocalizedText;
import org.eclipse.milo.opcua.stack.core.types.builtin.NodeId;
import org.eclipse.milo.opcua.stack.core.types.builtin.QualifiedName;
import org.eclipse.milo.opcua.stack.core.types.enumerated.MonitoringMode;
import org.eclipse.milo.opcua.stack.core.types.enumerated.TimestampsToReturn;
import org.eclipse.milo.opcua.stack.core.types.structured.EndpointDescription;
import org.eclipse.milo.opcua.stack.core.types.structured.MonitoredItemCreateRequest;
import org.eclipse.milo.opcua.stack.core.types.structured.MonitoringParameters;
import org.eclipse.milo.opcua.stack.core.types.structured.ReadValueId;
import org.eclipse.milo.opcua.stack.core.types.structured.ServerStatusDataType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static org.eclipse.milo.opcua.stack.core.types.builtin.unsigned.Unsigned.uint;

@SuppressWarnings("Duplicates")
public class MClient
{
    private final Logger logger = LoggerFactory.getLogger(getClass());
    private static final String ENDPOINT_URL = "opc.tcp://192.168.179.28:4840/";
//    private volatile boolean stop = false;

    public static void main(String[] args)
    {
        final MClient client = new MClient();
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

            }
            catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

            final NodeId nodeId = new NodeId(1, "Seconds");
//            final VariableNode variableNode = client.getAddressSpace().createVariableNode(nodeId);
//
//            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
//                stop = true;
//            }));
//
//            while (!stop) {
//                final DataValue dataValue = variableNode.readValue().get();
//                logger.info("{} = {}", nodeId, dataValue.getValue().getValue());
//            }

            final UaSubscription subscription = client.getSubscriptionManager().createSubscription(1000.0).get();
            final ReadValueId readValueId = new ReadValueId(nodeId, AttributeId.Value.uid(), null, QualifiedName.NULL_VALUE);
            final MonitoringParameters parameters = new MonitoringParameters(uint(1L), 1000.0, null, uint(10), true);
            final MonitoredItemCreateRequest request = new MonitoredItemCreateRequest(readValueId, MonitoringMode.Reporting, parameters);
            final List<UaMonitoredItem> monitoredItems = subscription.createMonitoredItems(TimestampsToReturn.Both,
                    Collections.singletonList(request),
                    (uaMonitoredItem, integer) -> uaMonitoredItem.setValueConsumer(this::onSubscriptionValue)).get();
            for (UaMonitoredItem item : monitoredItems) {
                if (item.getStatusCode().isGood()) {
                    logger.info("item created for nodeId={}", item.getReadValueId().getNodeId());
                } else {
                    logger.warn("failed to create item for nodeId={} (status={})", item.getReadValueId().getNodeId(), item.getStatusCode());
                }
            }

            final CompletableFuture<Void> future = new CompletableFuture<>();
            Runtime.getRuntime().addShutdownHook(new Thread(() -> future.complete(null)));
            logger.info("Waiting for shutdown");
            future.get();
            logger.info("Exit");

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

    private void onSubscriptionValue(UaMonitoredItem item, DataValue dataValue)
    {
        logger.info("{} = {}", item.getReadValueId().getNodeId(), dataValue.getValue().getValue());
    }
}

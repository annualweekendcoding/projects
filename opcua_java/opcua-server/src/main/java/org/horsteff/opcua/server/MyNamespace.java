/* Put your license comment here */
package org.horsteff.opcua.server;

import java.util.List;
import java.util.Locale;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;

import org.eclipse.milo.opcua.sdk.core.AccessLevel;
import org.eclipse.milo.opcua.sdk.core.Reference;
import org.eclipse.milo.opcua.sdk.server.OpcUaServer;
import org.eclipse.milo.opcua.sdk.server.api.AccessContext;
import org.eclipse.milo.opcua.sdk.server.api.DataItem;
import org.eclipse.milo.opcua.sdk.server.api.MethodInvocationHandler;
import org.eclipse.milo.opcua.sdk.server.api.MonitoredItem;
import org.eclipse.milo.opcua.sdk.server.api.Namespace;
import org.eclipse.milo.opcua.sdk.server.nodes.AttributeContext;
import org.eclipse.milo.opcua.sdk.server.nodes.ServerNode;
import org.eclipse.milo.opcua.sdk.server.nodes.UaFolderNode;
import org.eclipse.milo.opcua.sdk.server.nodes.UaMethodNode;
import org.eclipse.milo.opcua.sdk.server.nodes.UaVariableNode;
import org.eclipse.milo.opcua.sdk.server.nodes.delegates.DelegatingAttributeDelegate;
import org.eclipse.milo.opcua.sdk.server.util.AnnotationBasedInvocationHandler;
import org.eclipse.milo.opcua.stack.core.AttributeId;
import org.eclipse.milo.opcua.stack.core.Identifiers;
import org.eclipse.milo.opcua.stack.core.StatusCodes;
import org.eclipse.milo.opcua.stack.core.UaException;
import org.eclipse.milo.opcua.stack.core.types.builtin.DataValue;
import org.eclipse.milo.opcua.stack.core.types.builtin.LocalizedText;
import org.eclipse.milo.opcua.stack.core.types.builtin.NodeId;
import org.eclipse.milo.opcua.stack.core.types.builtin.QualifiedName;
import org.eclipse.milo.opcua.stack.core.types.builtin.StatusCode;
import org.eclipse.milo.opcua.stack.core.types.builtin.Variant;
import org.eclipse.milo.opcua.stack.core.types.builtin.unsigned.UShort;
import org.eclipse.milo.opcua.stack.core.types.enumerated.NodeClass;
import org.eclipse.milo.opcua.stack.core.types.enumerated.TimestampsToReturn;
import org.eclipse.milo.opcua.stack.core.types.structured.ReadValueId;
import org.eclipse.milo.opcua.stack.core.types.structured.WriteValue;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.common.collect.Lists;

import static org.eclipse.milo.opcua.stack.core.types.builtin.unsigned.Unsigned.ubyte;

public class MyNamespace implements Namespace
{
    static final String NAMESPACE_URI = "urn:horsteff:opcua:example";

    private final Logger logger = LoggerFactory.getLogger(getClass());
    private final OpcUaServer server;
    private final UShort namespaceIndex;

    MyNamespace(final OpcUaServer server, final UShort namespaceIndex)
    {
        this.server = server;
        this.namespaceIndex = namespaceIndex;

        final NodeId folderNodeId = new NodeId(namespaceIndex, "HelloWorld");
        final UaFolderNode folderNode = new UaFolderNode(server.getNodeMap(), folderNodeId,
                new QualifiedName(namespaceIndex, "HelloWorld"), german("Hello World!"));
        server.getNodeMap().addNode(folderNode);

        final String nodeName = "aValue";
        final NodeId typeId = Identifiers.Int32;
        final Variant variant = new Variant(12);

        final UaVariableNode node = UaVariableNode.builder(server.getNodeMap())
                .setNodeId(new NodeId(namespaceIndex, "HelloWorld/" + nodeName))
                .setAccessLevel(ubyte(AccessLevel.getMask(AccessLevel.READ_WRITE)))
                .setUserAccessLevel(ubyte(AccessLevel.getMask(AccessLevel.READ_WRITE)))
                .setBrowseName(new QualifiedName(namespaceIndex, nodeName))
                .setDisplayName(german(nodeName))
                .setDataType(typeId)
                .setTypeDefinition(Identifiers.BaseDataVariableType)
                .build();
        node.setValue(new DataValue(variant));
        node.setAttributeDelegate(new DelegatingAttributeDelegate() {});

        try {
            server.getUaNamespace().addReference(Identifiers.ObjectsFolder, Identifiers.Organizes, true,
                    folderNodeId.expanded(), NodeClass.Object);
            server.getNodeMap().addNode(node);
            folderNode.addOrganizes(node);
        }
        catch (UaException e) {
            logger.error("Error adding nodes: {}", e.getMessage(), e);
        }

        final UaMethodNode methodNode = UaMethodNode.builder(server.getNodeMap())
                .setNodeId(new NodeId(namespaceIndex, "HelloWorld/myMethod"))
                .setBrowseName(new QualifiedName(namespaceIndex, "myMethod"))
                .setDisplayName(german("My method"))
                .setDescription(german("gets a string and returns another one"))
                .build();

        try {
            final AnnotationBasedInvocationHandler invocationHandler =
                    AnnotationBasedInvocationHandler.fromAnnotatedObject(server.getNodeMap(), new MyMethod());
            methodNode.setInputArguments(invocationHandler.getInputArguments());
            methodNode.setOutputArguments(invocationHandler.getOutputArguments());
            methodNode.setInvocationHandler(invocationHandler);

            server.getNodeMap().addNode(methodNode);
        }
        catch (Exception e) {
            logger.error("Error adding method node: {}", e.getMessage(), e);
        }

    }

    @Override
    public UShort getNamespaceIndex()
    {
        return namespaceIndex;
    }

    @Override
    public String getNamespaceUri()
    {
        return NAMESPACE_URI;
    }

    @Override
    public void read(final ReadContext context, final Double maxAge, final TimestampsToReturn timestamps,
            final List<ReadValueId> readValueIds)
    {
        List<DataValue> results = Lists.newArrayListWithCapacity(readValueIds.size());

        for (ReadValueId readValueId : readValueIds) {
            ServerNode node = server.getNodeMap().get(readValueId.getNodeId());

            if (node != null) {
                DataValue value = node.readAttribute(new AttributeContext(context), readValueId.getAttributeId(),
                        timestamps, readValueId.getIndexRange(), readValueId.getDataEncoding());

                results.add(value);
                logger.info("Read value {} from attribut {}", value.getValue().getValue(), node.getNodeId());
            } else {
                results.add(new DataValue(StatusCodes.Bad_NodeIdUnknown));
                logger.warn("Read unknown node {}", readValueId.getNodeId());
            }
        }

        context.complete(results);
    }

    @Override
    public void write(final WriteContext context, final List<WriteValue> writeValues)
    {
        List<StatusCode> results = Lists.newArrayListWithCapacity(writeValues.size());

        for (WriteValue writeValue : writeValues) {
            ServerNode node = server.getNodeMap().get(writeValue.getNodeId());

            if (node != null) {
                try {
                    node.writeAttribute(new AttributeContext(context), writeValue.getAttributeId(),
                            writeValue.getValue(), writeValue.getIndexRange());
                    results.add(StatusCode.GOOD);

                    logger.info("Wrote value {} to {} attribute of {}", writeValue.getValue().getValue(),
                        AttributeId.from(writeValue.getAttributeId()).map(Object::toString).orElse("unknown"),
                        node.getNodeId());
                } catch (UaException e) {
                    logger.error("Unable to write value={}", writeValue.getValue(), e);
                    results.add(e.getStatusCode());
                }
            } else {
                results.add(new StatusCode(StatusCodes.Bad_NodeIdUnknown));
                logger.warn("Write to unknown node {}", writeValue.getNodeId());
            }
        }

        context.complete(results);
    }

    @Override
    public Optional<MethodInvocationHandler> getInvocationHandler(final NodeId methodId)
    {
        final Optional<ServerNode> node = server.getNodeMap().getNode(methodId);
        return node.flatMap(someNode -> {
            if (someNode instanceof UaMethodNode) {
                return ((UaMethodNode)someNode).getInvocationHandler();
            } else {
                return Optional.empty();
            }
        });
    }

    @Override
    public void onDataItemsCreated(final List<DataItem> list)
    {

    }

    @Override
    public void onDataItemsModified(final List<DataItem> list)
    {

    }

    @Override
    public void onDataItemsDeleted(final List<DataItem> list)
    {

    }

    @Override
    public void onMonitoringModeChanged(final List<MonitoredItem> list)
    {

    }

    @Override
    public CompletableFuture<List<Reference>> browse(final AccessContext accessContext, final NodeId nodeId)
    {
        return null;
    }

    private LocalizedText german(final String text)
    {
        return new LocalizedText(Locale.GERMAN.getLanguage(), text);
    }
}

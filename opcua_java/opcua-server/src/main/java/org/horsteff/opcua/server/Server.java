/* Put your license comment here */
package org.horsteff.opcua.server;

import java.util.Collections;
import java.util.EnumSet;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import org.eclipse.milo.opcua.sdk.server.OpcUaServer;
import org.eclipse.milo.opcua.sdk.server.api.config.OpcUaServerConfig;
import org.eclipse.milo.opcua.sdk.server.identity.UsernameIdentityValidator;
import org.eclipse.milo.opcua.sdk.server.util.HostnameUtil;
import org.eclipse.milo.opcua.stack.core.application.DefaultCertificateManager;
import org.eclipse.milo.opcua.stack.core.application.InsecureCertificateValidator;
import org.eclipse.milo.opcua.stack.core.security.SecurityPolicy;
import org.eclipse.milo.opcua.stack.core.types.builtin.LocalizedText;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Server
{
    private final Logger logger = LoggerFactory.getLogger(getClass());
    private final OpcUaServer opcUaServer;

    public static void main(String[] args) throws ExecutionException, InterruptedException
    {
        final Server server = new Server();
        final Logger logger = LoggerFactory.getLogger(server.getClass());

        logger.info("Startup");
        server.startup().get();
        logger.info("Running");

        final CompletableFuture<Void> future = new CompletableFuture<>();
        Runtime.getRuntime().addShutdownHook(new Thread(() -> future.complete(null)));
        logger.info("Waiting for shutdown");
        future.get();
        logger.info("Exit");
    }

    private Server()
    {
        final OpcUaServerConfig serverConfig = OpcUaServerConfig.builder()
                .setApplicationUri("urn:horsteff:opcua:example:server")
                .setApplicationName(LocalizedText.english("My OPC UA Example Server"))
                .setBindPort(12686)
                .setBindAddresses(Collections.singletonList("0.0.0.0"))
                .setEndpointAddresses(Collections.singletonList("localhost"))
                .setCertificateManager(new DefaultCertificateManager())
                .setCertificateValidator(new InsecureCertificateValidator())
                .setIdentityValidator(new UsernameIdentityValidator(true, authenticationChallenge -> true))
                .setServerName("example")
                .setSecurityPolicies(EnumSet.of(SecurityPolicy.None))
                .setUserTokenPolicies(Collections.singletonList(OpcUaServerConfig.USER_TOKEN_POLICY_ANONYMOUS))
                .build();

        opcUaServer = new OpcUaServer(serverConfig);

        opcUaServer.getNamespaceManager().registerAndAdd(MyNamespace.NAMESPACE_URI,
                index -> new MyNamespace(opcUaServer, index));
    }

    private CompletableFuture<OpcUaServer> startup()
    {
        return opcUaServer.startup();
    }
}

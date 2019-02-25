/* Put your license comment here */
package org.horsteff.opcua.server;

import org.eclipse.milo.opcua.sdk.server.annotations.UaInputArgument;
import org.eclipse.milo.opcua.sdk.server.annotations.UaMethod;
import org.eclipse.milo.opcua.sdk.server.annotations.UaOutputArgument;
import org.eclipse.milo.opcua.sdk.server.util.AnnotationBasedInvocationHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class MyMethod
{
    private final Logger logger = LoggerFactory.getLogger(getClass());

    @UaMethod
    public void invoke(AnnotationBasedInvocationHandler.InvocationContext context,
            @UaInputArgument(name = "s", description = "In string") final String s,
            @UaOutputArgument(name = "out_s", description = "Out string")
            final AnnotationBasedInvocationHandler.Out<String> outS)
    {
        logger.info("Invoking myMethod of object {}", context.getObjectNode().getBrowseName().getName());
        System.out.println("invoke(" + s + ")");
        outS.set("Hello out there!");
    }
}

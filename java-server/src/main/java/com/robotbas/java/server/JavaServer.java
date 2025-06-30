/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 */
package com.robotbas.java.server;

import io.grpc.Grpc;
import io.grpc.InsecureServerCredentials;
import io.grpc.Server;
import io.grpc.robotbas.devicesim.DeviceReply;
import io.grpc.robotbas.devicesim.DeviceRequest;
import io.grpc.robotbas.devicesim.DeviceServiceGrpc;
import io.grpc.stub.StreamObserver;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author berna
 */
public class JavaServer {

    private static final Logger logger = Logger.getLogger(JavaServer.class.getName());

    List<DeviceSimulator> devices;
    int numDevices = 1000;
    public JavaServer() {
    }
    
    private Server server;

    private void start() throws IOException {
        
        devices = new ArrayList<>();
        for (int i = 1; i <= numDevices; i++) {
            DeviceSimulator dev = new DeviceSimulator(String.format("TC7571.1.6.%d", i));
            devices.add(dev);
            dev.start();
        }
        
        /* The port on which the server should run */
        int port = 50051;
        /*
        * By default gRPC uses a global, shared Executor.newCachedThreadPool() for gRPC callbacks into
        * your application. This is convenient, but can cause an excessive number of threads to be
        * created if there are many RPCs. It is often better to limit the number of threads your
        * application uses for processing and let RPCs queue when the CPU is saturated.
        * The appropriate number of threads varies heavily between applications.
        * Async application code generally does not need more threads than CPU cores.
         */
        ExecutorService executor = Executors.newFixedThreadPool(2);
        server = Grpc.newServerBuilderForPort(port, InsecureServerCredentials.create())
                .executor(executor)
                .addService(new DeviceServiceImpl(devices))
                .build()
                .start();
        logger.info("Server started, listening on " + port);
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                // Use stderr here since the logger may have been reset by its JVM shutdown hook.
                System.err.println("*** shutting down gRPC server since JVM is shutting down");
                try {
                    JavaServer.this.stop();
                } catch (InterruptedException e) {
                    if (server != null) {
                        server.shutdownNow();
                    }
                    e.printStackTrace(System.err);
                } finally {
                    executor.shutdown();
                }
                System.err.println("*** server shut down");
            }
        });
        
    }

    private void stop() throws InterruptedException {
        if (server != null) {
            server.shutdown().awaitTermination(30, TimeUnit.SECONDS);
        }
        
        // Wait for all threads to finish
        for (DeviceSimulator dev: devices) {
            dev.join();
        }
    }

    /**
     * Await termination on the main thread since the grpc library uses daemon
     * threads.
     */
    private void blockUntilShutdown() throws InterruptedException {
        if (server != null) {
            server.awaitTermination();
        }
    }

    /**
     * Main launches the server from the command line.
     */
    public static void main(String[] args) throws IOException, InterruptedException {
        final JavaServer server = new JavaServer();
        server.start();

        server.blockUntilShutdown();
    }

    static class DeviceServiceImpl extends DeviceServiceGrpc.DeviceServiceImplBase {

        List<DeviceSimulator> devices;
        private final ReentrantLock lock = new ReentrantLock();  // Mutex
            
        public DeviceServiceImpl(List<DeviceSimulator> devices) {
            this.devices = devices;
        }
        @Override
        public synchronized void deviceVariablesValues(DeviceRequest req, StreamObserver<DeviceReply> responseObserver) {
            // Wait for all threads to finish
            for (DeviceSimulator dev: devices) {
                dev.subscribe(new DeviceSimulatorListener() {
                        @Override
                        public synchronized void onDeviceDataChange(DeviceData data) {
                            DeviceReply reply = DeviceReply.newBuilder()
                                    .setDeviceName(data.deviceName)
                                    .setTemperature(data.temperature)
                                .build();
                            lock.lock();
                            responseObserver.onNext(reply);
                            lock.unlock();
                        }
                    });
                
            }
            while(true) {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException ex) {
                    Logger.getLogger(JavaServer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }        
                
            // responseObserver.onCompleted();

        }
    }
}

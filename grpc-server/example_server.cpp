/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <unistd.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "absl/strings/str_format.h"

#include "protos/devicesim.grpc.pb.h"
#include "device_simulator.hpp"

using devicesim::DeviceReply;
using devicesim::DeviceRequest;
using devicesim::DeviceService;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

DeviceSimulator simulator;


// Logic and data behind the server's behavior.
class DeviceServiceImpl final : public DeviceService::Service
{
    Status DeviceVariablesValues(ServerContext *context, const DeviceRequest *request,
                                 ServerWriter<DeviceReply> *writer) override
    {

        std::cout << "Received request for DeviceVariablesValues" << std::endl;

        std::mutex write_mutex;
        simulator.onDataChanged.subscribe([writer, &write_mutex](EventData data)
                                          {
            DeviceReply reply;
            reply.set_device_name(data.device_name);
            reply.set_temperature(data.data.temperature);
            reply.set_humidity(data.data.humidity);
            reply.set_pressure(data.data.pressure);
            reply.set_timestamp(data.data.timestamp);
            std::lock_guard<std::mutex> lock(write_mutex);
            writer->Write(reply); 
        });

        while (true)
        {
            sleep(10);
        }
        return Status::OK;
    }
};

void RunServer(uint16_t port)
{
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    DeviceServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    std::vector<std::thread> threads;
    for (int i = 1; i < 1001; ++i) {
        std::string name = absl::StrFormat("TC7571.1.6.%d", i);
        threads.emplace_back(&DeviceSimulator::addDevice, &simulator, name);
    }
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();

    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

int main(int argc, char **argv)
{
    absl::ParseCommandLine(argc, argv);
    absl::InitializeLog();
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}

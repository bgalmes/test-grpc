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

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <thread>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "protos/devicesim.grpc.pb.h"

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

using devicesim::DeviceReply;
using devicesim::DeviceRequest;
using devicesim::DeviceService;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

class DeviceClient
{
public:
    DeviceClient(std::shared_ptr<Channel> channel)
        : stub_(DeviceService::NewStub(channel)) {}

    void DeviceVariablesValues()
    {

        // Data we are sending to the server.
        DeviceRequest request;
        // request.set_name(user);

        // Container for the data we expect from the server.
        DeviceReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::unique_ptr<ClientReader<DeviceReply>> reader(
            stub_->DeviceVariablesValues(&context, request));

        while (reader->Read(&reply))
        {
            std::cout << "Device: " << reply.device_name() << " - " << reply.temperature() << " at " << std::endl;
        }
        Status status = reader->Finish();
    }

private:
    std::unique_ptr<DeviceService::Stub> stub_;
};

int main(int argc, char **argv)
{
    absl::ParseCommandLine(argc, argv);
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    std::string target_str = absl::GetFlag(FLAGS_target);
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    DeviceClient client(
        grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    
    client.DeviceVariablesValues();
    // std::vector<std::thread> threads;
    // for (int i = 1; i < 200; ++i) {
    //     threads.emplace_back(&DeviceClient::DeviceVariablesValues, &client, "TC7571.1.6." + std::to_string(i));
    // }

    // // Wait for all threads to finish
    // for (auto& t : threads) {
    //     if (t.joinable()) {
    //         t.join();
    //     }
    // }


    return 0;
}

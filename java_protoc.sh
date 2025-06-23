mkdir -p java-server/src/main/proto

protoc --java_out=./java-server/src/main/proto ./protos/devicesim.proto
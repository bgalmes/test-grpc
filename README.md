# gRPC basic ussage

## Installation (Linux)

### Define installation folder
```bash
export GRPC_INSTALL_DIR=$HOME/.local
mkdir -p $GRPC_INSTALL_DIR
export PATH="$GRPC_INSTALL_DIR/bin:$PATH"
```

### Install cmake
```bash
sudo apt install -y cmake
```

See if required cmake version match with intalled one (Look at grpc docs).
```bash
cmake --version
```


### Install other required tools

```bash
sudo apt install -y build-essential autoconf libtool pkg-config
```

### Clone the grpc repo

```bash
git clone --recurse-submodules -b v1.73.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
```

### Build and install gRPC and Protocol Buffers 

```bash
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_INSTALL_PREFIX=$GRPC_INSTALL_DIR \
      ../..
make -j 4
make install
popd
```

### Update your user .bashrc file

Add the next lines at the end of your .bashrc file:
```bash
export GRPC_INSTALL_DIR=$HOME/.local
export PATH="$GRPC_INSTALL_DIR/bin:$PATH"
```
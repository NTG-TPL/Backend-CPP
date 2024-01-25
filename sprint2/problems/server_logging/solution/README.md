mkdir -p build-release

cd build-release

conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Release

cd ..

mkdir -p build-debug

cd build-debug

conan install .. --build=missing -s build_type=Debug -s compiler.libcxx=libstdc++11

cmake .. -DCMAKE_BUILD_TYPE=Debug

cd ..

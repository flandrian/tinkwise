mkdir -p build
pushd build
cmake -G "Unix Makefiles" ..
cpack
popd
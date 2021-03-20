cd native
mkdir cmake-build-debug
cd cmake-build-debug
rm -rf CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CACHEFILE_DIR=$PWD -Dcross_triple=$CROSS ..
cmake --build . --target native -- -j 3
ecode=$?

mkdir bin

cp *.dll bin
cp *.so bin

if [ $ecode != 0 ]; then
  exit $ecode
fi

cd /work

bash gradlew runCiTest

#Uncomment the following line and modify it's value if your libdl.so.2 is in a
#different location.
#LIBDL_PATH = -DLIBDL_PATH="/lib/libdl.so.2"

[ ! -d lib32 ] && ( rm -f lib32 ; mkdir lib32 )
[ ! -d lib64 ] && ( rm -f lib64 ; mkdir lib64 )

echo "--- 32bit ---"
gcc -m32 -Wall -fPIC -shared -ldl -o lib32/block_net.so block_net.c
echo "--- 64bit ---"
gcc -m64 -Wall -fPIC -shared -ldl -o lib64/block_net.so block_net.c


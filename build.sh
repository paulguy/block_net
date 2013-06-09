#Uncomment the following line and modify it's value if your libdl.so is in a
#different location.
#LIBDL_PATH = -DLIBDL_PATH="/lib/libdl.so.2"

#Uncomment the following line and modify it's value if your libc.so is in a
#different location.
#LIBDL_PATH = -DLIBDL_PATH="/lib/libc.so.6"

[ ! -d lib32 ] && ( rm -f lib32 ; mkdir lib32 )
[ ! -d lib64 ] && ( rm -f lib64 ; mkdir lib64 )

echo "--- 32bit ---"
gcc -m32 -Wall -fPIC -shared -ldl ${LIBDL_PATH} ${LIBC_PATH} -o lib32/block_net.so block_net.c
echo "--- 64bit ---"
gcc -m64 -Wall -fPIC -shared -ldl ${LIBDL_PATH} ${LIBC_PATH} -o lib64/block_net.so block_net.c


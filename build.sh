echo "--- 32bit ---"
gcc -m32 -Wall -fPIC -ggdb -shared -o lib32/block_net.so block_net.c
echo "--- 64bit ---"
gcc -m64 -Wall -fPIC -ggdb -shared -o lib64/block_net.so block_net.c


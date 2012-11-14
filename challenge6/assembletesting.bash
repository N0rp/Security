echo "Compiling shellcode"
gcc -c -o shellcode.o shellcode.s
objcopy -O binary shellcode.o shellcode.bin
objdump -d shellcode.o

gcc -o shellcode.o shellcode.s
chmod 0755 shellcode.o

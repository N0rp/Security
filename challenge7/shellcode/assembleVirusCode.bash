gcc -c -o viruscode.o viruscode.s
objcopy -O binary viruscode.o viruscode.bin
hexdump -C viruscode.bin | grep --color=auto 'ff\|00'

echo 'Running virus'
gcc -o viruscode.o viruscode.s
./viruscode.o

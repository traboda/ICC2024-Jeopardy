gdb-multiarch -ix gdbinit_realmode.txt \
-ex 'set architecture i8086' \
-ex 'target remote localhost:1234' \
-ex 'b *0x7C00' \
-ex 'b *0x7cd4' \
-ex 'c' \

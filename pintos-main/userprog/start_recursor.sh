make -j
time pintos -v -m 128 -k -T 60 qemu --filesys-size=4 \
    -p ../examples/recursor_ng -a recursor_ng \
    -- -q -f run 'recursor_ng pintosmaster 6 1'

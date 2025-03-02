# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF', <<'EOF']);
(read-bad-buf) begin
(read-bad-buf) open "sample.txt"
(read-bad-buf) end
read-bad-buf: exit(0)
EOF
(read-bad-buf) begin
(read-bad-buf) open "sample.txt"
read-bad-buf: exit(-1)
EOF
pass;

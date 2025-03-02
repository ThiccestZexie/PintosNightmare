# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(low-mem) begin
(low-mem) exec("child-simple"): -1
(low-mem) end
low-mem: exit(0)
EOF
pass;

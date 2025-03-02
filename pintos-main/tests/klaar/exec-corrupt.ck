# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF', <<'EOF', <<'EOF', <<'EOF', <<'EOF', <<'EOF']);
(exec-corrupt) begin
load: corrupt-elf: error loading executable
(exec-corrupt) exec("corrupt-elf"): -1
(exec-corrupt) end
exec-corrupt: exit(0)
EOF
(exec-corrupt) begin
(exec-corrupt) exec("corrupt-elf"): -1
(exec-corrupt) end
exec-corrupt: exit(0)
EOF
(exec-corrupt) begin
load: corrupt-elf: error loading executable
corrupt-elf: exit(-1)
(exec-corrupt) exec("corrupt-elf"): -1
(exec-corrupt) end
exec-corrupt: exit(0)
EOF
(exec-corrupt) begin
load: corrupt-elf: error loading executable
(exec-corrupt) exec("corrupt-elf"): -1
corrupt-elf: exit(-1)
(exec-corrupt) end
exec-corrupt: exit(0)
EOF
(exec-corrupt) begin
load: corrupt-elf: error loading executable
(exec-corrupt) exec("corrupt-elf"): -1
(exec-corrupt) end
exec-corrupt: exit(0)
corrupt-elf: exit(-1)
EOF
(exec-corrupt) begin
load: corrupt-elf: error loading executable
(exec-corrupt) exec("corrupt-elf"): -1
(exec-corrupt) end
corrupt-elf: exit(-1)
exec-corrupt: exit(0)
EOF
pass;

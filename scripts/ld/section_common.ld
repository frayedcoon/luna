. = ALIGN(4);
_terminal_start = .;
KEEP (*(SORT(.terminal.command.*)))
_terminal_end = .;

. = ALIGN(4);
_srv_start = .;
KEEP (*(SORT(.kernel.srv.*)))
_srv_end = .;

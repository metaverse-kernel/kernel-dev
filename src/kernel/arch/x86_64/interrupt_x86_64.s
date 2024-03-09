    section .text

    global interrupt_open
interrupt_open:
    sti
    ret

    global interrupt_close
interrupt_close:
    cli
    ret

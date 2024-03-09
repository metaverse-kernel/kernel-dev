#include <kernel/interrupt.h>
#include <utils.h>

#include <kernel/arch/x86_64/interrupt_procs.h>

void interrupt_init()
{                   
    gate_descriptor_t gate;
    trap_gate_generate(gate, interrupt_entry_sym(DE));
    interrupt_register_gate(gate, 0);
    trap_gate_generate(gate, interrupt_entry_sym(UNSUPPORTED));
    interrupt_register_gate(gate, 1);
    trap_gate_generate(gate, interrupt_entry_sym(NMI));
    interrupt_register_gate(gate, 2);
    trap_gate_generate(gate, interrupt_entry_sym(BP));
    interrupt_register_gate(gate, 3);
    interrupt_open();
}

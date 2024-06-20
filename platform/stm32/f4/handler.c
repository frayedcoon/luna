#include "kernel/thread.h"
#include "platform/usart.h"

extern void sv_schedule_routine(void*);

void usart2_handler(void) {
    extern usart_iface *usart_iface_2;
    if (usart_iface_2 && usart_iface_2->buffer_consume(usart_iface_2)) {
        thread_signal(usart_iface_2);
        sv_schedule_routine(NULL);
    }
}
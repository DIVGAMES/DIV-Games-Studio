; Timer interrupt handler

                .386
                .model flat

                .code

                public  _timer_code_lock_start
                public  _timer_code_lock_end
                public  _timer_oldvect
                public  _timer_count
                public  _timer_systemcount
                public  _timer_frequency
                public  _timer_ds
                public  _timer_function
                public  timer_handler_

_timer_code_lock_start:

                align   4

_timer_ds       dw      0
_timer_oldvect  df      0
_timer_count    dd      0
_timer_systemcount dw   0
_timer_frequency dw     0
_timer_function dd      0

                align   4


timer_handler_: pushad
                push    DS
                push    ES
                mov     DS, CS:_timer_ds
                mov     ES, CS:_timer_ds
                sti
                mov     AL, 20h
                out     20h, AL
                inc     _timer_count
                call    _timer_function
                mov     AX, _timer_frequency
                add     _timer_systemcount, AX
                jc      timer_callold
                pop     ES
                pop     DS
                popad
                iretd
timer_callold:  pop     ES
                pop     DS
                popad
                jmp     CS:_timer_oldvect

_timer_code_lock_end:

                end

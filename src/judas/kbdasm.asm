; Keyboard interrupt handler

                .386
                .model  FLAT

                .code

KUP             =       0
KDOWN           =       128
FALSE           =       0
TRUE            =       1

                public  _kt
                public  _kbd_code_lock_start
                public  _kbd_code_lock_end
                public  _kbd_ds
                public  kbd_handler_

                align   4

_kbd_code_lock_start:

_kbd_ds         dw      0
_kt             db      256 dup (KUP)
extendedkey     db      0
pausekey        db      0
xlattable       db      00h, 01h, 02h, 03h, 04h, 05h, 06h, 07h
                db      08h, 09h, 0ah, 0bh, 0ch, 0dh, 0eh, 0fh
                db      10h, 11h, 12h, 13h, 14h, 15h, 16h, 17h
                db      18h, 19h, 1ah, 1bh, 1ch, 9dh, 1eh, 1fh
                db      20h, 21h, 22h, 23h, 24h, 25h, 26h, 27h
                db      28h, 29h, 36h, 2bh, 2ch, 2dh, 2eh, 2fh
                db      30h, 31h, 32h, 33h, 34h, 35h, 36h, 37h
                db      0b8h, 39h, 3ah, 3bh, 3ch, 3dh, 3eh, 3fh
                db      40h, 41h, 42h, 43h, 44h, 45h, 46h, 0c7h
                db      0c8h, 0c9h, 4ah, 0cbh, 4ch, 0cdh, 4eh, 0cfh
                db      0d0h, 0d1h, 0d2h, 0d3h, 54h, 55h, 56h, 57h
                db      58h, 59h, 5ah, 5bh, 5ch, 5dh, 5eh, 5fh
                db      60h, 61h, 62h, 63h, 64h, 65h, 66h, 67h
                db      68h, 69h, 6ah, 6bh, 6ch, 6dh, 6eh, 6fh
                db      70h, 71h, 72h, 73h, 74h, 75h, 76h, 77h
                db      78h, 79h, 7ah, 7bh, 7ch, 7dh, 7eh, 7fh
                db      80h, 81h, 82h, 83h, 84h, 85h, 86h, 87h
                db      88h, 89h, 8ah, 8bh, 8ch, 8dh, 8eh, 8fh
                db      90h, 91h, 92h, 93h, 94h, 95h, 96h, 97h
                db      98h, 99h, 9ah, 9bh, 1ch, 9dh, 9eh, 9fh
                db      0a0h, 0a1h, 0a2h, 0a3h, 0a4h, 0a5h, 0a6h, 0a7h
                db      0a8h, 0a9h, 0aah, 0abh, 0ach, 0adh, 0aeh, 0afh
                db      0b0h, 0b1h, 0b2h, 0b3h, 0b4h, 0b5h, 0b6h, 0b7h
                db      0b8h, 0b9h, 0bah, 0bbh, 0bch, 0bdh, 0beh, 0bfh
                db      0c0h, 0c1h, 0c2h, 0c3h, 0c4h, 0c5h, 0c6h, 0c7h
                db      0c8h, 0c9h, 0cah, 0cbh, 0cch, 0cdh, 0ceh, 0cfh
                db      0d0h, 0d1h, 0d2h, 0d3h, 0d4h, 0d5h, 0d6h, 0d7h
                db      0d8h, 0d9h, 0dah, 0dbh, 0dbh, 0ddh, 0deh, 0dfh
                db      0e0h, 0e1h, 0e2h, 0e3h, 0e4h, 0e5h, 0e6h, 0e7h
                db      0e8h, 0e9h, 0eah, 0ebh, 0ech, 0edh, 0eeh, 0efh
                db      0f0h, 0f1h, 0f2h, 0f3h, 0f4h, 0f5h, 0f6h, 0f7h
                db      0f8h, 0f9h, 0fah, 0fbh, 0fch, 0fdh, 0feh, 0ffh

                align   4

kbd_handler_:   push    DS EAX EBX                      ;Save registers
                mov     AX, CS:_kbd_ds
                mov     DS, AX
                sti                                     ;As early as possible
                in      AL, 60h                         ;Read keycode
                cmp     pausekey, FALSE                 ;Pause codes left?
                je      kbd_nopause
                dec     pausekey                        ;Maybe next time...
                jmp     kbd_done
kbd_nopause:    cmp     AL, 0e0h                        ;Will the next be
                jne     kbd_noextended                  ;extended?
                mov     extendedkey, TRUE
                jmp     kbd_done
kbd_noextended: cmp     AL, 0e1h                        ;Is it the pause stream?
                jne     kbd_nopause2
                mov     byte ptr _kt+0ffh, KDOWN        ;Store Pause Key-Down
                mov     pausekey, 5                     ;There'll be five codes
                jmp     kbd_done                        ;coming to discard
kbd_nopause2:   mov     AH, AL
                and     AH, 80h                         ;AH holds the up/down
                xor     AH, 80h                         ;status
                and     AL, 7fh
                cmp     extendedkey, TRUE               ;Was this supposed to be
                jne     kbd_dontadd80                   ;an extended code?
                mov     extendedkey, FALSE
                cmp     AL, 2ah                         ;Skip "system request"
                je      kbd_done
                add     AL, 80h
kbd_dontadd80:  mov     EBX, offset xlattable
                xlat
                xor     EBX, EBX                        ;EBX is the key table
                mov     BL, AL                          ;index
                add     EBX, offset _kt
                mov     [EBX], AH
kbd_done:       mov     AL, 20h                         ;Non-specific end of
                out     20h, AL                         ;interrupt
                pop     EBX EAX DS                      ;Restore used registers
                iretd                                   ;Return from interrupt

_kbd_code_lock_end:

                end

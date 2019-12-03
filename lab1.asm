;tasm dos com #
.386p
.model tiny
pmode segment use16
    assume  cs:pmode, ds:pmode, es:pmode
    org 100h
start:
;------------------------------------------------------------------------
;------------------------------------------------------------------------
; Определяем селекторы как константы. У всех у них биты TI = 0 (выборка
;  дескрипторов производится из GDT), RPL = 00B - уровень привилегий -
;  нулевой.
 
Code_selector   =  8
Stack_selector  = 16
Data_selector   = 24
Screen_selector = 32
R_Mode_Code     = 40    ; Селектор дескриптора сегмента кода для возврата
                ;  в режим реальных адресов.
R_Mode_Data     = 48    ; Селектор дескриптора сегментов стека и данных.
;------------------------------------------------------------------------
; Сохраняем сегментные регистры, используемые в R-Mode:
    mov R_Mode_SS,ss
    mov R_Mode_DS,ds
    mov R_Mode_ES,es
    mov R_Mode_FS,fs
    mov R_Mode_GS,gs
; Подготавливаем адрес возврата в R-Mode:
    mov R_Mode_segment,cs
    lea ax,R_Mode_entry
    mov R_Mode_offset,ax
; Подготовка к переходу в защищённый режим:
    mov bx,offset GDT + 8
    xor eax,eax
    mov edx,eax
    push    cs
    pop ax
    shl eax,4
    mov dx,1024
    mov cl,10011000b
    call    set_descriptor      ; Code
    lea dx,Stack_seg_start
    add eax,edx
    mov dx,1024
    mov cl,10010110b
    call    set_descriptor      ; Stack
    xor eax,eax
    mov ax,ds
    shl eax,4
    mov dx,0ffffh
    mov cl,10010010b
    call    set_descriptor      ; Data
    mov eax,0b8000h
    mov edx,4000
    mov cl,10010010b
    call    set_descriptor      ; Screen
; Готовим дополнительные дескрипторы для возврата в R-Mode:
    xor eax,eax
    push    cs
    pop ax
    shl eax,4       ; EAX = физический адрес сегмента кода
                ;  (и всех остальных сегментов, т.к.
                ;  это .com-программа)
    mov edx,0ffffh
    mov cl,10011010b        ; P=1, DPL=00b, S=1, Тип=101b, A=0
    call    set_descriptor      ; R_Mode_Code 
    mov cl,10010010b        ; P=1, DPL=00b, S=1, Тип=001b, A=0
    call    set_descriptor      ; R_Mode_Data 
; Устанавливаем GDTR:
    xor eax,eax
    mov edx,eax
    mov ax,ds
    shl eax,4
    lea dx,GDT
    add eax,edx
    mov GDT_adr,eax
    mov dx,55       ; Предел GDT = 8 * (1 + 6) - 1
    mov GDT_lim,dx
    cli
    lgdt    GDTR
    mov R_Mode_SP,sp        ; Указатель на стек сохраняем в последний
                    ;  момент.
; Переходим в защищённый режим:
    mov eax,cr0
    or  al,1
    mov cr0,eax
; Процессор в защищённом режиме
    db  0eah        ; Команда far jmp Code_selector:P_Mode_entry.
    dw  P_Mode_entry
    dw  Code_selector
;------------------------------------------------------------------------
P_Mode_entry:
    mov ax,Screen_selector
    mov es,ax
    mov ax,Data_selector
    mov ds,ax
    mov ax,Stack_selector
    mov ss,ax
    mov sp,0

; Очистка экрана
	mov ax, 03
	int 10h
; Сообщаем о входе в P-Mode (выводим ZS-строку):	
    lea bx,Start_P_Mode_ZS
    mov di,480
    call    putzs
; Работа программы в защищённом режиме (здесь - только вывод строки):
    lea bx,P_Mode_ZS
    add di,160
    call    putzs 
; Возвращаемся в режим реальных адресов.
; 1. Запретить прерывания (CLI).
;    Прерывания уже запрещены при входе в P-Mode.
; 2. Передать управление в читаемый сегмент кода, имеющий предел в 64Кб.
    db  0eah        ; Команда far jmp R_Mode_Code:Pre_R_Mode_entry.
    dw  Pre_R_Mode_entry
    dw  R_Mode_Code
Pre_R_Mode_entry:
; 3. Загрузить в SS, DS, ES, FS и GS селекторы дескрипторов, имеющих
;    следующие параметры:
;  1) Предел = 64 Кб (FFFFh)
;  2) Байтная гранулярность (G = 0)
;  3) Расширяется вверх (E = 0)
;  4) Записываемый (W = 1)
;  5) Присутствующий (P = 1)
;  6) Базовый адрес = любое значение
 
    mov ax,R_Mode_Data  ; Селектор R_Mode_Data - "один на всех".
    mov ss,ax
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
 
; 4. Сбросить флаг PE в CR0.
 
    mov eax,cr0
    and al,0feh     ; FEh = 1111'1110b
    mov cr0,eax
 
; 5. Выполнить команду far jump на программу режима реальных адресов.
 
        db  0eah
R_Mode_offset   dw  ?   ; Значения R_Mode_offset и R_Mode_segment
R_Mode_segment  dw  ?   ;  сюда прописались перед входом в
                ;  защищённый режим (в начале программы).
;------------------------------------------------------------------------
R_Mode_entry:
; 6. Загрузить в регистры SS, DS, ES, FS и GS необходимые значения или 0
;    (восстанавливаем сохранённые значения):
 
    mov ss,R_Mode_SS
    mov ds,R_Mode_DS
    mov es,R_Mode_ES
    mov fs,R_Mode_FS
    mov gs,R_Mode_GS
 
    mov sp,R_Mode_SP        ; Восстанавливаем указатель стека
                    ;  непосредственно перед разрешением
                    ;  прерываний.
 
; 7. Разрешить прерывания (STI).
 
    sti
 
; Выводим ZS-строку "Back to real address mode..."
 
    lea bx,R_Mode_ZS
    mov ax,0b800h
    mov es,ax
    mov di,800
    call    putzs       ; Функция putzs универсальна и работает
                ;  в обоих режимах.
 
    int 20h     ; Конец программы (здесь - выход в MS-DOS).
;------------------------------------------------------------------------
; Вставка макросами кода функций, определённых в текстовом файле
;  "pmode.lib":
init_set_descriptor     macro
set_descriptor  proc    near
; Создаёт дескриптор.
; DS:BX = дескриптор в GDT
; EAX = адрес сегмента
; EDX = предел сегмента
; CL = байт прав доступа (access_rights)
 
    push    eax
    push    ecx     ; Регистры EAX и ECX мы будем использовать. 
    push    cx      ; Временно сохраняем значение access_rights.
    mov cx,ax       ; Копируем младшую часть адреса в CX,
    shl ecx,16      ; и сдвигаем её в старшую часть ECX.
    mov cx,dx       ; Копируем младшую часть предела в CX.
                ;  Теперь ECX содержит младшую часть
                ;  дескриптора (т.е. первые 4 байта -
                ;  см. рис. 4-1).
    mov [bx],ecx    ; Записываем младшую половину дескриптора в GDT.
    shr eax,16      ; EAX хранит адрес сегмента, младшую часть
                ;  которого мы уже использовали, теперь будем
                ;  работать со старшей, для чего сдвигаем её в
                ;  младшую часть EAX, т.е. в AX.
    mov cl,ah       ; Биты адреса с 24 по 31
    shl ecx,24      ; сдвигаем в старший байт ECX,
    mov cl,al       ; а биты адреса с 16 по 23 - в младший байт.
    pop ax      ; Возвращаем из стека в AX значение
                ;  access_rights
    mov ch,al       ; и помещаем его во второй (из четырёх)
                ;  байт ECX.
                ; Всё, дескриптор готов. Старшую часть
                ;  предела и биты GDXU мы не устанавливаем и
                ;  они будут иметь нулевые значения.
    mov [bx+4],ecx; Дописываем в GDT вторую половину дескриптора.
    add bx,8    ; Переводим указатель в GDT на следующий дескриптор.
    pop ecx
    pop eax
    ret
set_descriptor  endp
endm
putzs   proc    near
; DS:BX = ZS ;ZS = Zero-String - строка, оканчивающаяся нулевым (00h) байтом.
; ES:DI = позиция вывода, ES описывает сегмент видеопамяти, DI - смещение в нём.
    push    ax
    push    bx
    push    es
    push    di
    mov ah,1Bh ;В AH будет атрибут вывода - светло-циановые
                ;  символы на синем фоне.
putzs_1: mov al,[bx]    ; Читаем байт из ZS-строки.
    inc bx      ; Переводим указатель на следующий байт.
    cmp al,0        ; Если байт равен 0,
    je  putzs_end   ; то переходим в конец процедуры.
    mov es:[ di ],ax    ; Иначе - записываем символ вместе с
                ;  атрибутом в видеопамять по заданному
                ;  смещению - цветной символ появится на
                ;  экране.
    add di,2        ; Переводим указатель в видеопамяти на
                ;  позицию следующего символа.
    jmp putzs_1; Повторяем процедуру для следующего байта из ZS-строки.
 
putzs_end: pop di
    pop es
    pop bx
    pop ax
    ret
putzs endp
 
init_set_descriptor
;init_putzs
;------------------------------------------------------------------------
; ZS-строка для вывода при входе в P-Mode:
Start_P_Mode_ZS db  "Entering to protected mode...",0
 
; ZS-строка для вывода при работе в P-Mode:
P_Mode_ZS db    "Working in P-mode...",0
 
; ZS-строка для вывода в R-Mode:
R_Mode_ZS db    "Back to real address mode...",0
;------------------------------------------------------------------------
; Значения регистров, которые программа имела до перехода в P-Mode:
R_Mode_SP   dw  ?
R_Mode_SS   dw  ?
R_Mode_DS   dw  ?
R_Mode_ES   dw  ?
R_Mode_FS   dw  ?
R_Mode_GS   dw  ?
;------------------------------------------------------------------------
; Образ регистра GDTR:
 
GDTR    label   fword
GDT_lim     dw  ?
GDT_adr     dd  ?
;------------------------------------------------------------------------
GDT:
    dd  ?,? ; 0-й дескриптор
    dd  ?,? ; 1-й дескриптор (кода)
    dd  ?,? ; 2-й дескриптор (стека)
    dd  ?,? ; 3-й дескриптор (данных)
    dd  ?,? ; 4-й дескриптор (видеопамяти)
    dd  ?,? ; 5-й дескриптор (код для перехода в R-Mode)
    dd  ?,? ; 6-й дескриптор (стек и данные для перехода в R-Mode)
;------------------------------------------------------------------------
    db  1024 dup (?)    ; Зарезервировано для стека.
Stack_seg_start:    ; Последняя метка программы - отсюда будет расти стек.
;------------------------------------------------------------------------
    pmode   ends
    end start
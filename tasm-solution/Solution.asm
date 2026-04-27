.model small
.stack 100h
.data
    square     dw 100, 100, 6 dup(0)    ; (x1, y1, x2, y2, x3, y3, x4, y4)
    color      db 010b                  ; цвет
    border     db 111b                  ; обводка
    background db 000b                  ; фон
    speed      dw 10                    ; скорость
    step_x     dw -1                    ; по x (1 - вправо, -1 - влево)
    step_y     dw -1                    ; по y (1 - вниз, -1 - вверх)
    horizontal dw 320                   ; по горизонтали
    vertical   dw 200                   ; по вертикали
    area       dw 10                    ; размер квадрата
    seed       dw 13                    ; для генерации чисел
.code
main proc
                  mov  ax, @data
                  mov  ds, ax

    ; 11h	640x480	2
    ; 12h	640x480	16
    ; 13h	320x200	256
    ; Ввод:	АН = 0Ch
    ; ВН = номер видеостраницы
    ; DX = номер строки
    ; СХ = номер столбца
    ; AL = номер цвета (для режимов 10h и llh, если старший бит 1, номер цвета точки на экране будет результатом операции «исключающее ИЛИ»)
    ; Вывод:	Никакого
    ; INТ 10h AH = 0Dh — Считать точку с экрана AL - Цвет пикселя
                  mov  ax, 13h              ; режима VGA (320x200, 256 цветов)
                  int  10h

                  call create_square

    game_loop:    
    ; рисуем квадрат
                  mov  al, border
                  call draw
    
    ; закрашиваем
                  mov  al, color
                  call fill

    ; ожидание
                  call delay
                  call delay
                  call delay
                  call delay



    ; очищаем
                  mov  al, background
                  call draw

                  mov  al, background
                  call fill

    ; новые координаты
                  call update

                  jmp  game_loop
main endp

draw proc
    ;цвет в al
                  mov  ah, 0ch              ; функция отрисовки пикселя
                  mov  bh, 0                ; страница экрана

    ; рисуем верхнюю сторону квадрата
                  mov  cx, [square]
                  mov  dx, [square+2]
    draw_top:     
                  int  10h
                  inc  cx
                  cmp  cx, [square+4]
                  jle  draw_top

    ; рисуем нижнюю сторону квадрата
                  mov  cx, [square+8]
                  mov  dx, [square+10]
    draw_bottom:  
                  int  10h
                  inc  cx
                  cmp  cx, [square+12]
                  jle  draw_bottom

    ; рисуем левую сторону квадрата
                  mov  cx, [square]
                  mov  dx, [square+2]
    draw_left:    
                  int  10h
                  inc  dx
                  cmp  dx, [square+10]
                  jle  draw_left

    ; рисуем правую сторону квадрата
                  mov  cx, [square+4]
                  mov  dx, [square+6]
    draw_right:   
                  int  10h
                  inc  dx
                  cmp  dx, [square+14]
                  jle  draw_right

                  ret
draw endp

fill proc
    ;цвет в al
                  mov  ah, 0ch
                  mov  bh, 0

                  mov  cx, [square]
                  add  cx, 1
                  mov  dx, [square+2]
                  add  dx, 1

    loop_y:       
                  mov  si, cx
    loop_x:       
                  int  10h
                  inc  cx
                  cmp  cx, [square+4]
                  jl   loop_x

                  mov  cx, si
                  inc  dx
                  cmp  dx, [square+10]
                  jl   loop_y

                  ret
fill endp

update proc
                  mov  si, 0

    update_loop:  
    ; x1...x4
                  mov  ax, speed
                  mov  bx, step_x
                  imul bx
                  add  ax, [square+si]
                  mov  [square+si], ax

    ;столкновение с границами по x
                  cmp  ax, 0
                  jl   create_square
                  cmp  ax, [horizontal]
                  jg   create_square
    ; y1...y4
                  mov  ax, speed
                  mov  bx, step_y
                  imul bx
                  add  ax, [square+si+2]
                  mov  [square+si+2], ax

    ;столкновение с границами по y
                  cmp  ax, 0
                  jl   create_square
                  cmp  ax, [vertical]
                  jg   create_square
    ; следующая пара
                  add  si, 4
                  cmp  si, 16
                  jl   update_loop

                  ret
update endp

create_square proc
    ;случайное смещение для всех координат
                  call rnd_num
                  mov  bx, ax

    ; рандомные координаты
                  call rnd_pos

    ; случайное направление
                  call rnd_step
                  mov  step_x, ax
                  call rnd_step
                  mov  step_y, ax

                  ret
create_square endp

rnd_pos proc
    ; безопасной позиции x
                  mov  ax, bx
                  mov  cx, [area]
                  add  cx, ax               ; смещение расстояния от левой границы
                  cmp  cx, [horizontal]     ; не выйти за правую границу
                  jl   correct_x
                  sub  cx, [area]
    correct_x:    
                  mov  [square], cx         ; x1 (верхний левый угол)
                  mov  [square+8], cx       ; x3 (нижний левый угол)
                  add  cx, [area]
                  mov  [square+4], cx       ; x2 (верхний правый угол)
                  mov  [square+12], cx      ; x4 (нижний правый угол)


    ; безопасной позиции y
                  mov  ax, bx
                  mov  dx, [area]
                  add  dx, ax               ; смещение расстояния от верхней границы
                  cmp  dx, [vertical]       ; не выйти за нижнюю границу
                  jl   correct_y
                  sub  dx, [area]
    correct_y:    
                  mov  [square+2], dx       ; y1 (верхний левый угол)
                  mov  [square+6], dx       ; y2 (верхний правый угол)
                  add  dx, [area]
                  mov  [square+10], dx      ; y3 (нижний левый угол)
                  mov  [square+14], dx      ; y4 (нижний правый угол)

                  ret
rnd_pos endp

rnd_num proc
                  mov  ax, seed
                  mov  bx, 3
                  imul bx
                  add  ax, 3
                  mov  seed, ax
                  and  ax, 0FFh             ; не более 255
                  ret
rnd_num endp

rnd_step proc
                  call rnd_num
                  and  ax, 1
                  cmp  ax, 0
                  je   pos_step
                  mov  ax, 1
                  ret
    pos_step:     
                  mov  ax, -1
                  ret
rnd_step endp

delay proc
                  mov  cx, 0FFFFh
    delay_loop:   
                  loop delay_loop
                  ret
delay endp

end main
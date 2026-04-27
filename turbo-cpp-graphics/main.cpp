#include <iostream.h>
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>

#define MAX_STACK_SIZE 2000 //  Максимальный размер стека

// Структура для представления точки
struct Point {
    double x, y;
};

struct Stack {
    Point data[MAX_STACK_SIZE];
    int top;

    Stack() : top(-1) {}

    int isEmpty() { return top == -1; }
    int isFull() { return top == MAX_STACK_SIZE - 1; }

    void push(Point p) {
        if (!isFull()) {
            data[++top] = p;
        }
    }

    Point pop() {
        if (!isEmpty()) {
            return data[top--];
        }
        // Обработка пустого стека
        Point center;
        center.x = 99999;
        center.y = 99999;
        return center;
    }
};

//Цифровой Дифференциальный Анализатор
void DDALine(Point p1, Point p2, int color) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    double steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0) { // Особый случай: отрезок длины 0
        putpixel(int(p1.x), int(p1.y), color);
        return;
    }

    double xIncrement = dx / steps;
    double yIncrement = dy / steps;

    double x = p1.x;
    double y = p1.y;

    for (int i = 0; i <= steps; ++i) {
        putpixel(int(x), int(y), color);
        x += xIncrement;
        y += yIncrement;
    }
}

void swap(Point &p1, Point &p2) {
    Point temp = p1;
    p1 = p2;
    p2 = temp;
}

// Алгоритм растеризации отрезка с использованием уравнения прямой
void drawLineA(Point p1, Point p2, int color) {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;

        if (abs(dx) > abs(dy)) {  // Если угол наклона меньше 45 градусов
            if (p1.x > p2.x) { // Обеспечиваем, что p1.x <= p2.x
                swap(p1, p2);
            }

            double m = dy / dx;
            for (double x = p1.x; x <= p2.x; x++) {
                double y = p1.y + m * (x - p1.x);
                putpixel(int(x), int(y), color);
            }
        } else { // Если угол наклона больше или равен 45 градусов
            if (p1.y > p2.y) { // Обеспечиваем, что p1.y <= p2.y
                swap(p1, p2);
            }

            double m = dx / dy;
            for (double y = p1.y; y <= p2.y; y++) {
                double x = p1.x + m * (y - p1.y);
                putpixel(int(x), int(y), color);
            }
        }
    }

//Алгоритм растеризации отрезка (Брезенхем)
void drawLineBrezenhem(Point p1, Point p2, int color) {
    int x = int(p1.x);
    int y = int(p1.y);

    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);

    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2;

    while (1) {
        putpixel(x, y, color);

        if (x == int(p2.x) && y == int(p2.y)) {
            break;
        }

        int e2 = err;

        if (e2 > -dx) {
            err -= dy;
            x += sx;
        }

        if (e2 < dy) {
            err += dx;
            y += sy;
        }
    }
}

// Функция для закраски с использованием 4-связной области recursive
void floodFill4(int x, int y, int fillColor, int borderColor,int* pixelCount) {
    if (x < 0 || x >= getmaxx() || y < 0 || y >= getmaxy() ||
        getpixel(x, y) == fillColor || getpixel(x, y) == borderColor) {
        return;
        }

    putpixel(x, y, fillColor);
    (*pixelCount)++;

    floodFill4(x + 1, y, fillColor, borderColor,pixelCount);
    floodFill4(x - 1, y, fillColor, borderColor,pixelCount);
    floodFill4(x, y + 1, fillColor, borderColor,pixelCount);
    floodFill4(x, y - 1, fillColor, borderColor,pixelCount);
}

// Функция для закраски с использованием 8-связной области stack
int floodFill8(int x, int y, int fillColor, int borderColor) {
    Stack s;
    int pixelCount = 0;

    // Изначальная точка
    Point point;
    point.x = x;
    point.y = y;

    s.push(point);

    while (!s.isEmpty()) {
        Point p = s.pop();

        x = p.x;
        y = p.y;

        // Проверка выхода за границы экрана
        if (x < 0 || x >= getmaxx() || y < 0 || y >= getmaxy()) {
            continue;
        }

        // Проверка цвета пикселя (если это уже залитый или граничный пиксель)
        int pixelColor = getpixel(x, y);
        if (pixelColor == fillColor || pixelColor == borderColor || pixelColor != BLACK) {
            continue;
        }

        // Закрашиваем пиксель
        putpixel(x, y, fillColor);
        pixelCount++;

        // Создаем и добавляем соседние пиксели в стек, проверяя границы
        Point point1;
        point1.x = x + 1; point1.y = y;   // вправо
        if (getpixel(point1.x, point1.y) == BLACK && getpixel(point1.x, point1.y) != borderColor) {
            s.push(point1);
        }

        Point point2;
        point2.x = x - 1; point2.y = y;   // влево
        if (getpixel(point2.x, point2.y) == BLACK && getpixel(point2.x, point2.y) != borderColor) {
            s.push(point2);
        }

        Point point3;
        point3.x = x; point3.y = y + 1;   // вниз
        if (getpixel(point3.x, point3.y) == BLACK && getpixel(point3.x, point3.y) != borderColor) {
            s.push(point3);
        }

        Point point4;
        point4.x = x; point4.y = y - 1;   // вверх
        if (getpixel(point4.x, point4.y) == BLACK && getpixel(point4.x, point4.y) != borderColor) {
            s.push(point4);
        }

        Point point5;
        point5.x = x + 1; point5.y = y + 1; // вправо-вниз
        if (getpixel(point5.x, point5.y) == BLACK && getpixel(point5.x, point5.y) != borderColor) {
            if (getpixel(x, y + 1) == BLACK || getpixel(x + 1, y) == BLACK) {
                s.push(point5);
            }
        }

        Point point6;
        point6.x = x - 1; point6.y = y - 1; // влево-вверх
        if (getpixel(point6.x, point6.y) == BLACK && getpixel(point6.x, point6.y) != borderColor) {
            if (getpixel(x, y - 1) == BLACK || getpixel(x - 1, y) == BLACK) {
                s.push(point6);
            }
        }

        Point point7;
        point7.x = x + 1; point7.y = y - 1; // вправо-вверх
        if (getpixel(point7.x, point7.y) == BLACK && getpixel(point7.x, point7.y) != borderColor) {
            if (getpixel(x, y - 1) == BLACK || getpixel(x + 1, y) == BLACK) {
                s.push(point7);
            }
        }

        Point point8;
        point8.x = x - 1; point8.y = y + 1; // влево-вниз
        if (getpixel(point8.x, point8.y) == BLACK && getpixel(point8.x, point8.y) != borderColor) {
            if (getpixel(x, y + 1) == BLACK || getpixel(x - 1, y) == BLACK) {
                s.push(point8);
            }
        }
    }
    return pixelCount;
}


// Рекурсивная функция для закраски с использованием 8-связной области
void floodFill8Recursive(int x, int y, int fillColor, int borderColor,int* pixelCount) {
    // Проверка выхода за границы экрана
    if (x < 0 || x >= getmaxx() || y < 0 || y >= getmaxy()) {
        return;
    }

    // Проверка цвета пикселя (если это уже залитый или граничный пиксель)
    int pixelColor = getpixel(x, y);
    if (pixelColor == fillColor || pixelColor == borderColor || pixelColor != BLACK) {
        return;
    }

    // Закрашиваем пиксель
    putpixel(x, y, fillColor);
    (*pixelCount)++;

    // Рекурсивный вызов для соседних пикселей
    // Вправо
    floodFill8Recursive(x + 1, y, fillColor, borderColor,pixelCount);
    // Влево
    floodFill8Recursive(x - 1, y, fillColor, borderColor,pixelCount);
    // Вниз
    floodFill8Recursive(x, y + 1, fillColor, borderColor,pixelCount);
    // Вверх
    floodFill8Recursive(x, y - 1, fillColor, borderColor,pixelCount);

    // Вправо-вниз
    if (getpixel(x + 1, y) == BLACK || getpixel(x, y + 1) == BLACK) {
        floodFill8Recursive(x + 1, y + 1, fillColor, borderColor,pixelCount);
    }

    // Влево-вверх
    if (getpixel(x - 1, y) == BLACK || getpixel(x, y - 1) == BLACK) {
        floodFill8Recursive(x - 1, y - 1, fillColor, borderColor,pixelCount);
    }

    // Вправо-вверх
    if (getpixel(x + 1, y) == BLACK || getpixel(x, y - 1) == BLACK) {
        floodFill8Recursive(x + 1, y - 1, fillColor, borderColor,pixelCount);
    }

    // Влево-вниз
    if (getpixel(x - 1, y) == BLACK || getpixel(x, y + 1) == BLACK) {
        floodFill8Recursive(x - 1, y + 1, fillColor, borderColor,pixelCount);
    }
}



// Функция для отрисовки многоугольника
void drawPolygon(Point* vertices, int numVertices, int color, void (*drawLine)(Point, Point, int)) {
    for (int i = 0; i < numVertices; i++) {
        drawLine(vertices[i], vertices[(i + 1) % numVertices], color);
    }
}

// Функция для отображения границ и координатных осей
void drawAxesAndBorders() {

    int maxX = getmaxx()-20;  // Ширина экрана
    int maxY = getmaxy()-20;  // Высота экрана

    // Рисуем рамку вокруг экрана (границы)
    rectangle(0, 0, maxX, maxY);

    line(0, maxY / 2, maxX, maxY / 2);  // (ось X)
    outtextxy(maxX - 20, maxY / 2 + 5, "X");  // Подпись "X"

    line(maxX / 2, 0, maxX / 2, maxY);  // (ось Y)
    outtextxy(maxX / 2 + 5, 10, "Y");  // Подпись "Y"

    char labelX[10];
    for (int i = 0; i < maxX; i += 50) {
        // Рисуем метки по оси X
        line(i, maxY / 2 - 5, i, maxY / 2 + 5);
        // Выводим текст с координатами на оси X
        itoa(i - maxX / 2, labelX, 10);
        outtextxy(i + 5, maxY / 2 + 5, labelX);
    }

    char labelY[10];
    for (i = 0; i < maxY; i += 50) {
        // Рисуем метки по оси Y
        line(maxX / 2 - 5, i, maxX / 2 + 5, i);
        // Выводим текст с координатами на оси Y
        itoa(maxY / 2 - i, labelY, 10);
        outtextxy(maxX / 2 + 5, i - 10, labelY);
    }
}

// Функция для вычисления длины отрезка
float calculateLength(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Функция для вычисления угла наклона отрезка относительно оси X
float calculateAngle(Point p1, Point p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x) * 180 / M_PI;  // Преобразуем в градусы
}

void displaySegmentInfo(Point p1, Point p2) {
    // Вычисляем длину и угол
    int length = calculateLength(p1, p2);
    int angle = calculateAngle(p1, p2);

    // Создаем буфер для текста
    char lengthText[20], angleText[20];

    // Преобразуем длину и угол в строку с помощью gcvt
    gcvt(length, 2, lengthText);  // Конвертируем длину в строку с 2 знаками после запятой
    gcvt(angle, 2, angleText);    // Конвертируем угол в строку с 2 знаками после запятой

    // Находим середину отрезка
    int midX = (p1.x + p2.x) / 2;
    int midY = (p1.y + p2.y) / 2;

    // Рассчитываем смещение текста на основе длины отрезка
    int offsetX = 0, offsetY = 0;
    int dist = calculateLength(p1, p2);

    // Увеличиваем смещение в зависимости от длины отрезка
    if (dist < 50) {
        offsetX = 30;
        offsetY = 30;
    } else if (dist < 100) {
        offsetX = 40;
        offsetY = 40;
    } else if (dist < 200) {
        offsetX = 50;
        offsetY = 50;
    } else {
        offsetX = 60;
        offsetY = 60;
    }

    // Отображаем длину и угол рядом с центром отрезка
    setcolor(WHITE);

    // Для длины (отклоняем немного по Y для уменьшения наложения)
    outtextxy(midX + offsetX, midY - 15 + offsetY, "Length: ");
    outtextxy(midX + offsetX + 60, midY - 15 + offsetY, lengthText);  // Смещаем текст, чтобы он не перекрывал метку

    // Для угла (отклоняем по Y для увеличения расстояния)
    outtextxy(midX + offsetX, midY + 15 + offsetY, "Angle: ");
    outtextxy(midX + offsetX + 60, midY + 15 + offsetY, angleText);  // Смещаем текст, чтобы он не перекрывал метку
}


int moveImage(double &offsetX, double &offsetY, double &scale, int &exitFlag,int &gridEnabled) {
    char ch = getch();  // Получаем код нажатой клавиши

    // В зависимости от нажатой клавиши изменяем смещение
    switch (ch) {
        case 122:  // Z
            offsetX -= scale;  // Сдвигаем изображение влево
        break;
        case 120:  // X
            offsetX += scale;  // Сдвигаем изображение вправо
        break;
        case 99:   // C
            offsetY -= scale;  // Сдвигаем изображение вверх
        break;
        case 118:  // V
            offsetY += scale;  // Сдвигаем изображение вниз
        break;
        case 98:   // B
            scale = (scale >= 25) ? 25 : scale + 5;
        break;
        case 110:  // N
            scale = (scale > 5) ? scale - 5 : 5;  // Уменьшаем масштаб (не меньше 5)
        break;
        case 109:  // M
            gridEnabled = 1 - gridEnabled;
        break;
        case 27:   // Esc для выхода
            exitFlag = 0;  // Устанавливаем флаг завершения программы
        break;
    }
    return 1;
}

void clearScreen() {
    cout << "\033[2J\033[H";  // ANSI escape
}

unsigned long long rdtsc() {
    unsigned long long tsc;
    asm {
        db 0x0F    // Прямой код инструкции для RDTSC
        db 0x31
        mov WORD PTR tsc, ax
        mov WORD PTR tsc+2, dx
    }
    return tsc;
}


// Функция для проведения экспериментов по растеризации линий
void runLineExperiment(Point start, Point end, int color, void (*drawLineFunc)(Point, Point, int)) {
    cleardevice();
    drawAxesAndBorders();

    unsigned long long startTime, endTime;
    double timeTaken = 0;
    int delta = 1;

    setcolor(BLACK);
    bar(10, 10, 10 + textwidth("Time taken: 999999"), 10 + textheight("Time taken: "));

    for (int i = 0; i < 1000; ++i) {
        startTime = rdtsc();
        drawLineFunc(start, end, color);
        endTime = rdtsc();
        timeTaken += (endTime - startTime) / (4.0 * 1000000000);

        start.x += delta;
        if (start.x >= getmaxx() || start.x <= 0) {
            delta = -delta;  // Изменяем направление, если вышли за границы
        }
    }

    char buffer[50];
    gcvt(timeTaken, 6, buffer);
    outtextxy(10, 10, "Time taken: ");
    outtextxy(10 + textwidth("Time taken: "), 10, buffer);

    displaySegmentInfo(start, end);
    getch();
}

Point calculatePolygonCenter(const Point points[], int numPoints) {
    double centerX = 0, centerY = 0;
    for (int i = 0; i < numPoints; ++i) {
        centerX += points[i].x;
        centerY += points[i].y;
    }
    centerX /= numPoints;
    centerY /= numPoints;

    Point center;
    center.x = centerX;
    center.y = centerY;
    return center;
}

void drawGrid(int scale, double offsetX, double offsetY) {
    int maxX = getmaxx()-20;
    int maxY = getmaxy()-20;

    setcolor(DARKGRAY);

    for (int x = 0; x <= maxX; x += scale) {
        line(x + int(offsetX)% int(scale) , 0, x + int(offsetX)% int(scale) , maxY);
    }

    for (int y = 0; y <= maxY; y += scale) {
        line(0, y + int(offsetY)% int(scale) , maxX, y + int(offsetY)% int(scale) );
    }

}

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    double scale = 20.0;
    double offsetX = getmaxx() / 4.0;
    double offsetY = getmaxy() / 4.0;
    int exitFlag = 1;
    int mode = 0;
    int gridEnabled = 0;

    while (exitFlag) {
        if (mode == 0) {
            // Все 12 многоугольников с их координатами и цветами
            Point polygon1[] = {{-0.1, 7}, {0.1, 7}, {0.1, 1.5}, {-0.1, 1.5}};
            Point polygon2[] = {{0.1, 3}, {0.6, 4.1}, {1.5, 4.3}, {1.3, 3.4}};
            Point polygon3[] = {{-0.1, 1.8}, {-1.4, 2.2}, {-1.8, 3.4}, {-0.6, 3}};
            Point polygon4[] = {{1, 9}, {3, 10}, {4, 10}, {5, 9}, {4, 8}, {3, 8}};
            Point polygon5[] = {{0, 8}, {1, 6}, {1, 5}, {0, 4}, {-1, 5}, {-1, 6}};
            Point polygon6[] = {{-1, 9}, {-3, 8}, {-4, 8}, {-5, 9}, {-4, 10}, {-3, 10}};
            Point polygon7[] = {{0, 10}, {-1, 12}, {-1, 13}, {0, 14}, {1, 13}, {1, 12}};
            Point polygon8[] = {{0, 10.5}, {1.5, 9}, {0, 7.5}, {-1.5, 9}};
            Point polygon9[] = {{0, 9}, {-1.5, 9.5}, {-1.5, 10.5}, {-0.5, 10.5}};
            Point polygon10[] = {{0, 9}, {0.5, 10.5}, {1.5, 10.5}, {1.5, 9.5}};
            Point polygon11[] = {{0, 9}, {1.5, 8.5}, {1.5, 7.5}, {0.5, 7.5}};
            Point polygon12[] = {{0, 9}, {-0.5, 7.5}, {-1.5, 7.5}, {-1.5, 8.5}};

            Point fillPoints[] = {
                {0.0, 4.5},  // Внутри polygon1
                {0.8, 3.8},  // Внутри polygon2
                {-1.0, 2.7}, // Внутри polygon3
                {3.0, 9.0},  // Внутри polygon4
                {0.0, 5.0},  // Внутри polygon5
                {-3.5, 9.0}, // Внутри polygon6
                {0.0, 12.5}, // Внутри polygon7
                {0.0, 9.0},  // Внутри polygon8
                {-1.0, 10.0},// Внутри polygon9
                {1.0, 9.5},  // Внутри polygon10
                {1.0, 8.0},  // Внутри polygon11
                {-1.0, 8.0}  // Внутри polygon12
            };

            Point* polygons[] = {polygon1, polygon2, polygon3, polygon4, polygon5, polygon6, polygon7, polygon8, polygon9, polygon10, polygon11, polygon12};
            int numVertices[] = {4, 4, 4, 6, 6, 6, 6, 4, 4, 4, 4, 4};
            int colors[] = {GREEN, GREEN, YELLOW, WHITE, WHITE, WHITE, WHITE, BROWN, YELLOW, YELLOW, YELLOW, YELLOW};
            int numPolygons = sizeof(polygons) / sizeof(polygons[0]);
            clearScreen();

            // Масштабирование, смещение и центрирование
            for (int i = 0; i < numPolygons; ++i) {
                for (int j = 0; j < numVertices[i]; ++j) {
                    polygons[i][j].x = polygons[i][j].x * scale + offsetX;
                    polygons[i][j].y = polygons[i][j].y * scale + offsetY;
                }

                drawPolygon(polygons[i], numVertices[i], colors[i], drawLineA);

                int fillX = int(fillPoints[i].x * scale + offsetX);
                int fillY = int(fillPoints[i].y * scale + offsetY);
                int pixels=0;
                floodFill4(fillX, fillY, colors[i], colors[i],&pixels);

                for (int k = 0; k < numVertices[i]; ++k) {
                    Point p1 = polygons[i][k];
                    Point p2 = polygons[i][(k + 1) % numVertices[i]];  // Следующая точка
                    //displaySegmentInfo(p1, p2);
                }
            }

            drawAxesAndBorders();
            if (gridEnabled) {
                drawGrid(int(scale), offsetX, offsetY);
            }

            moveImage(offsetX, offsetY, scale, exitFlag,gridEnabled);
        }
        else {
            Point start = {1, 2};
            Point end = {7, 10};
            start.x = start.x * scale + offsetX;
            start.y = start.y * scale + offsetY;
            end.x = end.x * scale + offsetX;
            end.y = end.y * scale + offsetY;

            // Точка внутри многоугольника для начала заливки
            Point polygon[] = {{0, 10}, {-1, 12}, {-1, 13}, {0, 14}, {1, 13}, {1, 12}};
            int numVertices = 6;
            Point center = calculatePolygonCenter(polygon, numVertices);

            clearScreen();
            cout << "Experiment Mode (Press 'e' to return to normal mode)\n";
            cout << "1. Iterative Line Algorithm\n";
            cout << "2. DDA Line Algorithm\n";
            cout << "3. Bresenham's Line Algorithm\n";
            cout << "4. floodFill4\n";
            cout << "5. floodFill8\n";
            cout << "6. floodFill8Recursive\n";
            cout << "Choose an algorithm (1-6): ";
            char choice;
            cin >> choice;
            switch (choice) {
                case '1': runLineExperiment(start, end, WHITE, drawLineA); break;
                case '2': runLineExperiment(start, end, WHITE, DDALine); break;
                case '3': runLineExperiment(start, end, WHITE, drawLineBrezenhem); break;
                case '4':
                    cleardevice();
                    unsigned long long startTime, endTime;
                    double timeTaken = 0;

                    for (int j = 0; j < numVertices; ++j) {
                        polygon[j].x = polygon[j].x * scale + offsetX;
                        polygon[j].y = polygon[j].y * scale + offsetY;
                    }
                    drawPolygon(polygon, numVertices, 5, drawLineA);
                    int fillX = int(center.x * scale + offsetX);
                    int fillY = int(center.y * scale + offsetY);
                    int pixels=0;
                    startTime = rdtsc();
                    floodFill4(fillX, fillY, 5, 5,&pixels);
                    endTime = rdtsc();
                    timeTaken += (endTime - startTime) / (4.0 * 1000000000);

                    for (int k = 0; k < numVertices; ++k) {
                        Point p1 = polygon[k];
                        Point p2 = polygon[(k + 1) % numVertices];
                        displaySegmentInfo(p1, p2);
                    }
                    char buffer[50];
                    gcvt(timeTaken, 6, buffer);
                    outtextxy(10, 30, "Fill Time taken: ");
                    outtextxy(10 + textwidth("Fill Time taken: "), 30, buffer);
                    outtextxy(10 + textwidth("Fill Time taken: ") + textwidth(buffer), 30, " seconds");

                    char pixelCountBuffer[50];
                    itoa(pixels, pixelCountBuffer, 10);
                    outtextxy(10, 50, "Pixels filled: ");
                    outtextxy(10 + textwidth("Pixels filled: "), 50, pixelCountBuffer);
                break;
                case '5':
                    cleardevice();
                    startTime, endTime;
                    timeTaken = 0;

                    for (j = 0; j < numVertices; ++j) {
                        polygon[j].x = polygon[j].x * scale + offsetX;
                        polygon[j].y = polygon[j].y * scale + offsetY;
                    }
                    drawPolygon(polygon, numVertices, 5, drawLineA);
                    fillX = int(center.x * scale + offsetX);
                    fillY = int(center.y * scale + offsetY);

                    startTime = rdtsc();
                    pixels = floodFill8(fillX, fillY, 5, 5);
                    endTime = rdtsc();
                    timeTaken += (endTime - startTime) / (4.0 * 1000000000);

                    for (k = 0; k < numVertices; ++k) {
                        Point p1 = polygon[k];
                        Point p2 = polygon[(k + 1) % numVertices];
                        displaySegmentInfo(p1, p2);
                    }
                    buffer[50];
                    gcvt(timeTaken, 6, buffer);
                    outtextxy(10, 30, "Fill Time taken: ");
                    outtextxy(10 + textwidth("Fill Time taken: "), 30, buffer);
                    outtextxy(10 + textwidth("Fill Time taken: ") + textwidth(buffer), 30, " seconds");

                    pixelCountBuffer[50];
                    itoa(pixels, pixelCountBuffer, 10);
                    outtextxy(10, 50, "Pixels filled: ");
                    outtextxy(10 + textwidth("Pixels filled: "), 50, pixelCountBuffer);

                    break;
                case '6':
                    cleardevice();
                    startTime, endTime;
                    timeTaken = 0;

                    for (j = 0; j < numVertices; ++j) {
                        polygon[j].x = polygon[j].x * scale + offsetX;
                        polygon[j].y = polygon[j].y * scale + offsetY;
                    }
                    drawPolygon(polygon, numVertices, 5, drawLineA);
                    fillX = int(center.x * scale + offsetX);
                    fillY = int(center.y * scale + offsetY);

                    startTime = rdtsc();
                    pixels = 0;
                    floodFill8Recursive(fillX, fillY, 5, 5,&pixels);
                    endTime = rdtsc();
                    timeTaken += (endTime - startTime) / (4.0 * 1000000000);

                    for (k = 0; k < numVertices; ++k) {
                        Point p1 = polygon[k];
                        Point p2 = polygon[(k + 1) % numVertices];
                        displaySegmentInfo(p1, p2);
                    }
                    buffer[50];
                    gcvt(timeTaken, 6, buffer);
                    outtextxy(10, 30, "Fill Time taken: ");
                    outtextxy(10 + textwidth("Fill Time taken: "), 30, buffer);
                    outtextxy(10 + textwidth("Fill Time taken: ") + textwidth(buffer), 30, " seconds");

                    pixelCountBuffer[50];
                    itoa(pixels, pixelCountBuffer, 10);
                    outtextxy(10, 50, "Pixels filled: ");
                    outtextxy(10 + textwidth("Pixels filled: "), 50, pixelCountBuffer);
                break;
                default: cout << "Invalid choice.\n";
            }
        }
        char ch = getch();
        if (ch == 'e') {
            mode = 1 - mode;
            cleardevice();
            if (mode == 0){
                offsetX = getmaxx() / 4.0;
                offsetY = getmaxy() / 4.0;
                scale = 20;
            }
        }
        delay(100);
    }
    closegraph();
    return 0;
}
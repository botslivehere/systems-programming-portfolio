#include <bitset>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>

#pragma pack(push, 1)
struct Header {
    uint16_t width;
    uint16_t height;
    uint8_t bitsPerPixel;
    uint16_t paletteSize;
};
#pragma pack(pop)

struct ARGB {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

bool loadFromFile(const std::string& filename, Header& header, std::vector<ARGB>& palette, std::vector<int8_t>& pixelData) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file!" << std::endl;
        return false;
    }

    // Чтение заголовка
    file.read(reinterpret_cast<char*>(&header), sizeof(Header));

    // Чтение палитры
    palette.resize(header.paletteSize);
    for (int i = 0; i < header.paletteSize; ++i) {
        file.read(reinterpret_cast<char*>(&palette[i]), sizeof(ARGB));
    }

    // Проверка заголовка
    if (header.bitsPerPixel != 5 || header.paletteSize != 15) {
        std::cerr << "Error: Unsupported file format!" << std::endl;
        return false;
    }

    // Рассчитать размер пиксельных данных
    int totalBits = header.width * header.height * header.bitsPerPixel;
    int pixelDataSize = ((totalBits + 7) / 8);
    pixelData.resize(pixelDataSize);

    file.read(reinterpret_cast<char*>(pixelData.data()), pixelDataSize);
    file.close();

    return true;
}

std::vector<ARGB> convertPixelData(const std::vector<int8_t>& pixelData, const std::vector<ARGB>& palette) {
    std::vector<ARGB> finalPixels;
    unsigned int currentBits = 0;
    int bitOffset = 0;

    // Распаковка пикселей, упакованных по парам
    for (size_t i = 0; i < pixelData.size(); ++i) {
        currentBits = (currentBits << 8) | (static_cast<unsigned char>(pixelData[i]));
        bitOffset += 8;

        while (bitOffset >= 5) {
            unsigned char colorIndex = (currentBits >> (bitOffset - 5)) & 0x1F;

            std::cout << "CurrentBits: " << std::bitset<32>(currentBits)
                      << ", BitOffset: " << bitOffset
                      << ", Extracted ColorIndex: " << static_cast<int>(colorIndex) << std::endl;

            // Проверка допустимости индекса
            if (colorIndex >= palette.size()) {
                std::cerr << "Error: Invalid color index! Index = " << static_cast<int>(colorIndex) << std::endl;
                colorIndex=14;
            }
                finalPixels.push_back(palette[colorIndex]);
                bitOffset -= 5; // Уменьшаем bitOffset на 5, так как мы извлекли 5 бит
        }
    }

    return finalPixels;
}

void printFileInfo(const Header& header, const std::vector<ARGB>& palette, const std::vector<int8_t>& pixelData,std::vector<ARGB> finalPixels) {
    std::cout << "Image size: " << header.width << "x" << header.height << std::endl;
    std::cout << "Bits per pixel: " << static_cast<int>(header.bitsPerPixel) << std::endl;
    std::cout << "Palette size: " << header.paletteSize << std::endl;

    // Выводим палитру
    std::cout << "Palette:" << std::endl;
    for (int i = 0; i < palette.size(); ++i) {
        std::cout << "Color " << i + 1 << ": "
                  << "A: " << static_cast<int>(palette[i].alpha) << " "
                  << "R: " << static_cast<int>(palette[i].red) << " "
                  << "G: " << static_cast<int>(palette[i].green) << " "
                  << "B: " << static_cast<int>(palette[i].blue) << std::endl;
    }


    // Выводим распакованные пиксели
    std::cout << "Pixel data (converted):" << std::endl;
    for (const auto& pixel : finalPixels) {
        std::cout << "A: " << static_cast<int>(pixel.alpha) << " "
                  << "R: " << static_cast<int>(pixel.red) << " "
                  << "G: " << static_cast<int>(pixel.green) << " "
                  << "B: " << static_cast<int>(pixel.blue) << std::endl;
    }
}

HBITMAP createBitmapFromPixels(HDC hdc, const std::vector<ARGB>& pixels, int width, int height) {
    // Создаем битмап в памяти
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HDC hMemDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldBitmap = SelectObject(hMemDC, hBitmap);

    // Заполняем битмап пикселями
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const ARGB& pixel = pixels[y * width + x];
            COLORREF color = RGB(pixel.red, pixel.green, pixel.blue);
            SetPixel(hMemDC, x, y, color);
        }
    }

    // Освобождаем ресурсы
    SelectObject(hMemDC, oldBitmap);
    DeleteDC(hMemDC);

    return hBitmap;
}

static float zoomFactor = 1.0f;

void drawGrid(HDC hdc, int width, int height, float zoomFactor, int offsetX, int offsetY) {
    // Цвет сетки (серый)
    HPEN hPen = CreatePen(PS_SOLID, 0.025*zoomFactor, RGB(200, 200, 200));
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    // Убедимся, что сетка рисуется точно по границам пикселей
    for (int x = 0; x <= width; ++x) {
        int xPos = static_cast<int>((x * zoomFactor) + offsetX);
        MoveToEx(hdc, xPos, 0 + offsetY, nullptr);
        LineTo(hdc, xPos, static_cast<int>(height * zoomFactor) + offsetY);
    }

    for (int y = 0; y <= height; ++y) {
        int yPos = static_cast<int>((y * zoomFactor) + offsetY);
        MoveToEx(hdc, 0 + offsetX, yPos, nullptr);
        LineTo(hdc, static_cast<int>(width * zoomFactor) + offsetX, yPos);
    }

    // Восстановим старую кисть
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

bool saveToFile(const std::string& filename, const Header& header, const std::vector<ARGB>& palette, const std::vector<int8_t>& pixelData) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing!" << std::endl;
        return false;
    }

    // Запись заголовка
    file.write(reinterpret_cast<const char*>(&header), sizeof(Header));

    // Запись палитры
    for (const auto& color : palette) {
        file.write(reinterpret_cast<const char*>(&color), sizeof(ARGB));
    }

    // Запись данных пикселей
    file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());

    file.close();
    return true;
}


static std::vector<HWND> buttonHandles;
static HWND hScaleInput;

void createButtons(HWND hwnd, const char* buttonLabels[], int numberOfButtons) {
    for (HWND btn : buttonHandles) {
        DestroyWindow(btn);
    }
    DestroyWindow(hScaleInput);
    buttonHandles.clear();

    int buttonWidth = 100;
    int buttonHeight = 50;
    int spacing = 10;

    RECT clientRect;
    GetClientRect(hwnd, &clientRect); // Получаем размеры клиентской области окна

    hScaleInput = CreateWindow("EDIT", "100", // Default text
                                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                clientRect.right - 110, 10, 100, 20,
                                hwnd, nullptr, nullptr, nullptr);

    // Расчет начальной Y позиции для кнопок
    int y = clientRect.bottom - (numberOfButtons * buttonHeight + (numberOfButtons - 1) * spacing);
    int x = clientRect.right - buttonWidth - 10; // Отступ от правого края

    for (int i = 0; i < numberOfButtons; ++i) {
        HWND hButton = CreateWindow("BUTTON", buttonLabels[i],
                                     WS_VISIBLE | WS_CHILD,
                                     x, y, buttonWidth, buttonHeight,
                                     hwnd, reinterpret_cast<HMENU>(1001 + i), nullptr, nullptr);
        buttonHandles.push_back(hButton);
        y += buttonHeight + spacing;
    }
}


static bool showGrid = false;  // Переменная для контроля видимости сетки
static int offsetX = 0; // Offset for horizontal
static int offsetY = 0; // Offset for vertical

static const char* buttonLabels[] = { "Grid", "Scale UP", "Scale DOWN", "UP", "DOWN", "LEFT", "RIGHT","Set Scale","Save" };
static int buttonCount = sizeof(buttonLabels) / sizeof(buttonLabels[0]);

// Окно процедуры
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static Header header;
    static std::vector<ARGB> palette;
    static std::vector<int8_t> pixelData;
    static std::vector<ARGB> finalPixels;

    switch (uMsg) {
        case WM_CREATE:
            if (loadFromFile("C:/Users/avdzh/CLionProjects/untitled/input3.bin", header, palette, pixelData)) {
                finalPixels = convertPixelData(pixelData, palette);
                printFileInfo(header, palette, pixelData, finalPixels);
                createButtons(hwnd, buttonLabels, buttonCount);
    }
        break;
        case WM_SIZE: {
            createButtons(hwnd, buttonLabels, buttonCount);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Очистка фона окна
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));  // Белый цвет фона
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);

            if (!finalPixels.empty()) {
                // Создаем битмап из пикселей
                HBITMAP hBitmap = createBitmapFromPixels(hdc, finalPixels, header.width, header.height);

                // Создаем контекст памяти для битмапа
                HDC hMemDC = CreateCompatibleDC(hdc);
                HGDIOBJ oldBitmap = SelectObject(hMemDC, hBitmap);

                // Масштабируем битмап с помощью StretchBlt
                int scaledWidth = static_cast<int>(header.width * zoomFactor);
                int scaledHeight = static_cast<int>(header.height * zoomFactor);
                StretchBlt(hdc, offsetX, offsetY, scaledWidth, scaledHeight, hMemDC, 0, 0, header.width, header.height, SRCCOPY);

                // Рисуем сетку
                if (showGrid) {
                    drawGrid(hdc, header.width, header.height, zoomFactor, offsetX, offsetY);
                }

                // Освобождаем ресурсы
                SelectObject(hMemDC, oldBitmap);
                DeleteObject(hBitmap);
                DeleteDC(hMemDC);
            }

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_COMMAND: {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            // Проверяем идентификатор элемента управления (кнопки)
            switch (wParam) {
                case 1001: // Ловим нажатие кнопки
                    showGrid = !showGrid;
                    InvalidateRect(hwnd, nullptr, TRUE);
                break;
                case 1002:
                    zoomFactor *= 2;
                    InvalidateRect(hwnd, nullptr, TRUE);
                break;
                case 1003:
                    zoomFactor = std::max(1.f, zoomFactor /2 );
                    InvalidateRect(hwnd, nullptr, TRUE);
                break;
                case 1004:  // UP
                    if (offsetY > 0) {
                        offsetY -= 10;
                        InvalidateRect(hwnd, nullptr, TRUE);
                    }
                break;
                case 1005: // DOWN
                    if (offsetY < (height - static_cast<int>(header.height * zoomFactor))) {
                        offsetY += 10;
                        InvalidateRect(hwnd, nullptr, TRUE);
                    }
                break;
                case 1006: // LEFT
                    if (offsetX > 0) {
                        offsetX -= 10;
                        InvalidateRect(hwnd, nullptr, TRUE);
                    }
                break;
                case 1007: // RIGHT
                    if (offsetX < (width - static_cast<int>(header.width * zoomFactor))) {
                        offsetX += 10;
                        InvalidateRect(hwnd, nullptr, TRUE);
                    }
                break;
                case 1009:
                    saveToFile("C:/Users/avdzh/CLionProjects/untitled/output.bin", header, palette, pixelData);
                    break;
                case 1008: //Scale
                    char scaleText[10];
                    GetWindowText(hScaleInput, scaleText, sizeof(scaleText));
                    int scalePercentage = std::atoi(scaleText);
                    if (scalePercentage >= 1 && scalePercentage <= 1000000) {
                        zoomFactor = scalePercentage / 100.0f;
                    }
                    InvalidateRect(hwnd, nullptr, TRUE);
                    break;
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Pixel Display",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, nCmdShow);

    // Основной цикл сообщений
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0,0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
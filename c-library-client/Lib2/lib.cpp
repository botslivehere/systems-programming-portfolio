#define _CRT_SECURE_NO_WARNINGS
#include "lib.h"

ifstream intaxes("taxes01.txt");

ifstream inhome("home01.txt");

void readHome(long long* a, long long* b)
{
    assert(intaxes && "taxes.txt not found");

    long long temp_counter = 0; /// temp_counter -> количество квартир в доме.

    long long temp_ploshad = 0; /// temp_ploshad -> общая площадь всех (жилых и нежилых) помещений в доме.

    inhome >> temp_counter >> temp_ploshad;

    *a = temp_counter;

    *b = temp_ploshad;
}

void readHome2(long long* a, struct house_info* house)
{
    long long temp_counter = *a; /// temp_counter -> количество квартир в доме полученные при первом считывании и возвращенные через указатель.

    for (long long i = 0; temp_counter > i; i++)
    {
        inhome >> house[i].hous_ploshad >> house[i].hous_people;
    }
    inhome.close();
}

void readTaxes(long long* a)
{
    assert(inhome && "home.txt not found");

    long long taxes_Q = 0; /// taxes_Q -> количество различных ресурсов, которые потребляют жильцы дома.

    intaxes >> taxes_Q;

    *a = taxes_Q;
}

void readTaxes2(long long* a, char* b, long long* c, struct taxes_info* tax)
{
    long long taxes_Q = *a; /// taxes_Q -> количество различных ресурсов, которые потребляют жильцы дома,полученные при первом считывании и возвращенные через указатель.

    long long curent_pos = 0; /// curent_pos -> для подсчёта позиции нужного тарифа на ресурс.

    long long len_b = len(b); ///len_b -> получение длины слова при помощи указателя *b на массив типа char.

    for (long long i = 0; taxes_Q > i; ++i)
    {
        intaxes >> tax[i].taxes_resourse >> tax[i].taxes_tarif >> tax[i].taxes_const;

        if (len(tax[i].taxes_resourse) == len_b)
        {
            char temp[6];           /// temp -> для временного использования слова из структуры taxes_info.
            long long alp_find = 0; /// alp_find -> для подсчёта совпавших букв.
            strcpy(temp, tax[i].taxes_resourse);
            for (long long j = 0; len_b > j; j++)
            {
                if (temp[j] == *(b + j))
                {
                    alp_find++;
                }
            }
            if (alp_find == len_b)
            {
                curent_pos = i;
            }
        }
    }
    *c = curent_pos;
    intaxes.close();
}

long long len(char* b)
{
    long long len_Tb = 0; /// len_Tb -> для подсчёта количества символов в массиве типа char до символа конца массива '\0' и его исключая.

    while (*b != '\0')
    {
        len_Tb++;
        *b++;
    }
    return len_Tb;
}
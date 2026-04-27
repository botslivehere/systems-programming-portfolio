/*--------------------------------------------------------------------------------
Институт компьютерных технологий и информационной безопасности ЮФУ
Кафедра МОП ЭВМ
Основы алгоритмизации и программирования
Индивидуальное задание.
Исполнитель:Авджян Артур Эдуардович (КТбо1-9)
10.01.2022
----------------------------------------------------------------------------------*/

#include <iostream>
#include "lib.h"
#include "lib2.h"

int main()
{
    char znak; /// znak -> для считывания знаков между переменными.

    char Resource[6]; /// Resource[6] -> для считывания Тарифа из потока inMain.

    long long Flat = 0;       /// Flat -> для считывания Номера квартиры.
    long long StartMonth = 0; /// StartMonth -> для считывания начального месяца.
    long long StartYear = 0;  /// StartYear -> для считывания начального года.
    long long EndMonth = 0;   /// EndMonth -> для считывания конечного месяца.
    long long EndYear = 0;    /// EndYear -> для считывания конечного года.

    ifstream inMain("input01.txt");

    assert(inMain && "input.txt not found");

    inMain >> Flat >> Resource >> StartMonth >> znak >> StartYear >> znak >> EndMonth >> znak >> EndYear;

    inMain.close();

    long long house_counter = 0;     /// house_counter -> для считывания количества квартир в доме.
    long long house_ploshad = 0;     /// house_ploshad -> для считывания площади дома.
    long long diff_resourse = 0;     /// diff_resourse -> для считывания количество различных ресурсов.
    long long resource_position = 0; /// resource_position -> для получения позиции нужного ресурса.

    long long* PT_house_counter = &house_counter;
    long long* PT_house_ploshad = &house_ploshad;
    long long* PT_diff_resourse = &diff_resourse;
    char* PT_Resource = Resource;
    long long* PT_Resource_position = &resource_position;

    readHome(PT_house_counter, PT_house_ploshad);

    struct house_info* house = (struct house_info*)calloc(house_counter, sizeof(struct house_info));

    readHome2(PT_house_counter, house);

    readTaxes(PT_diff_resourse);

    struct taxes_info* taxi = (struct taxes_info*)calloc(diff_resourse, sizeof(struct taxes_info));

    readTaxes2(PT_diff_resourse, PT_Resource, PT_Resource_position, taxi);

    ifstream inDetails("details01.txt");

    assert(inDetails && "details.txt not found");

    long long List_all = 0;     /// List_all -> для считывания количества показателей ОДПУ.
    long long List_private = 0; /// List_private -> для считывания количества показателей ИПУ.

    inDetails >> List_all;

    struct ODPY_info* detail_a = (struct ODPY_info*)calloc(List_all, sizeof(struct ODPY_info));

    for (long long i = 0; List_all > i; i++)
    {
        inDetails >> detail_a[i].Month >> znak >> detail_a[i].Year;

        long long temp = 0; /// temp -> для считывания тарифных значений ОДПУ.

        for (long long j = 0; diff_resourse > j; j++)
        {
            inDetails >> temp;

            if (j == resource_position)
            {
                detail_a[i].Current_ODPY = temp;
            }
        }
    }

    inDetails >> List_private;

    struct IPY_info* detail_b = (struct IPY_info*)calloc(List_private, sizeof(struct IPY_info));

    for (long long i = 0; List_private > i; i++)
    {
        inDetails >> detail_b[i].Month >> znak >> detail_b[i].Year >> detail_b[i].Flat_Number;

        long long temp = 0; /// temp -> для считывания тарифных значений ИПУ.

        for (long long j = 0; diff_resourse > j; j++)
        {
            inDetails >> temp;

            if (j == resource_position)
            {
                detail_b[i].Current_IPY = temp;
            }
        }
    }

    inDetails.close();

    long long SumExpFlat = 0;  /// SumExpFlat -> суммарное потребление заданного в запросе ресурса (на основании показаний ИПУ)
    long long SumCostFlat = 0; /// SumCostFlat -> суммарное начисление за потребленный ресурс.
    long long SumExpODN = 0;   /// SumExpODN -> суммарное потребление заданного в запросе ресурса, относящееся на ОДН.
    long long SumCostODN = 0;  /// SumCostODN -> суммарное начисление за ОДН по данному ресурсу.

    long long SumMonthStart = StartMonth + StartYear * 12; /// SumMonthStart -> Количество всех месяцев у начальной даты.
    long long SumMonthEnd = EndMonth + EndYear * 12; /// SumMonthStart -> Количество всех месяцев у конечной даты.
    long long sumFlatIPY = 0; /// sumFlatIPY -> Сумма всех разниц ИПУ(i) и ИПУ(i-1) от начальной до конечной.

    qsort(detail_b, List_private, sizeof(IPY_info), cmp_IPY);

    qsort(detail_a, List_all, sizeof(ODPY_info), cmp_ODPY);

    struct Result* Result_info = (struct Result*)calloc(List_private, sizeof(struct Result));

    long long count_a = 0; /// count_a -> Счетчик записей ИПУ.
    long long count_b = 0; /// count_b -> Счетчик записей ОДПУ.
    long long ALL_people = 0; /// ALL_people -> Количество жильцов.

    for (long long d = 0; house_counter > d; d++)
    {
        ALL_people += house[d].hous_people;
    }

    for (long long i = SumMonthStart; SumMonthEnd >= i; i++)
    {
        for (long long j = 0; List_private > j; j++)
        {
            if ((detail_b[j].Year * 12 + detail_b[j].Month == i) && (detail_b[j].Flat_Number == Flat))
            {
                Result_info[count_a].Month = (i - 1) % 12 + 1;

                Result_info[count_a].Year = (i - 1) / 12;

                Result_info[count_a].ExpFlat = detail_b[j].Current_IPY - detail_b[j - 1].Current_IPY;

                if ((detail_b[j].Year * 12 + detail_b[j].Month) == (9 + 2021 * 12))
                {

                    Result_info[count_a].ExpFlat = 0;
                }

                Result_info[count_a].CostFlat = Result_info[count_a].ExpFlat * taxi[resource_position].taxes_tarif;

                SumExpFlat += Result_info[count_a].ExpFlat;

                SumCostFlat += Result_info[count_a].CostFlat;

                count_a++;
            }
        }

        sumFlatIPY = 0;

        for (long long l = 0; List_private > l; l++)
        {
            if (detail_b[l].Year * 12 + detail_b[l].Month == i)
            {
                sumFlatIPY += detail_b[l].Current_IPY - detail_b[l - 1].Current_IPY;
            }
        }

        for (long long k = 0; List_all > k; k++)
        {
            if (detail_a[k].Year * 12 + detail_a[k].Month == i)
            {
                Result_info[count_b].ExpODN = detail_a[k].Current_ODPY - detail_a[k - 1].Current_ODPY - sumFlatIPY;

                if (taxi[resource_position].taxes_const == 0)
                {

                    Result_info[count_b].ExpODN *= house[Flat - 1].hous_ploshad;

                    Result_info[count_b].ExpODN /= house_ploshad;
                }

                if (taxi[resource_position].taxes_const == 1)
                {

                    Result_info[count_b].ExpODN *= house[Flat - 1].hous_people;

                    Result_info[count_b].ExpODN /= ALL_people;
                }

                if ((detail_a[k].Year * 12 + detail_a[k].Month) == (9 + 2021 * 12))
                {

                    Result_info[count_b].ExpODN = 0;
                }

                Result_info[count_b].CostODN = Result_info[count_b].ExpODN * taxi[resource_position].taxes_tarif;

                SumExpODN += Result_info[count_b].ExpODN;

                SumCostODN += Result_info[count_b].CostODN;

                count_b++;
            }
        }
    }

    ofstream outMain("answer01.txt");

    outMain << SumExpFlat << ' ' << SumCostFlat << ' ' << SumExpODN << ' ' << SumCostODN << endl;
    outMain << count_a << endl;
    for (long long i = 0; count_a > i; i++)
    {
        outMain << setfill('0') << setw(2) << Result_info[i].Month << '.' << Result_info[i].Year
            << ' ' << Result_info[i].ExpFlat << ' ' << Result_info[i].CostFlat
            << ' ' << Result_info[i].ExpODN << ' ' << Result_info[i].CostODN << endl;
    }
    return 0;
}
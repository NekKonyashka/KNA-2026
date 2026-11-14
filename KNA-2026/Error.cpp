#include "Error.h"
#include "IT.h"

namespace Error {
	ERROR errors[ERROR_MAX_ENTRY] = {
		ERROR_ENTRY(0,"Недопустимый код ошибки"),
		ERROR_ENTRY(1,"Системный блок"),
		ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5),
		ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
		ERROR_ENTRY_NODEF10(10),ERROR_ENTRY_NODEF10(20),ERROR_ENTRY_NODEF10(30),ERROR_ENTRY_NODEF10(40),
		ERROR_ENTRY_NODEF10(50),ERROR_ENTRY_NODEF10(60),ERROR_ENTRY_NODEF10(70),ERROR_ENTRY_NODEF10(80),
		ERROR_ENTRY_NODEF10(90),
		ERROR_ENTRY(100,"Параметр -in должен быть задан"),
		ERROR_ENTRY_NODEF(101),ERROR_ENTRY_NODEF(102),ERROR_ENTRY_NODEF(103),
		ERROR_ENTRY(104,"Превышена длина входного параметра"),
		ERROR_ENTRY_NODEF(105),ERROR_ENTRY_NODEF(106),ERROR_ENTRY_NODEF(107),
		ERROR_ENTRY_NODEF(108),ERROR_ENTRY_NODEF(109),
		ERROR_ENTRY(110,"Ошибка при открытии файла с исходным кодом (-in)"),
		ERROR_ENTRY(111,"Недопустимый символ в исходном файле (-in)"),
		ERROR_ENTRY(112,"Ошибка при создании файла протокола (-log)"),
		ERROR_ENTRY(113, "Ошибка при создании таблицы лексем: превышена ёмкость таблицы лексем"),
		ERROR_ENTRY(114, "Ошибка при добавлении лексемы в таблицу: таблица лексем заполнена"),
		ERROR_ENTRY(115, "Ошибка при получении лексемы из таблицы: недопустимый номер лексемы"),
		ERROR_ENTRY(116, "Ошибка при создании таблицы идентификаторов: превышена ёмкость таблицы идентификаторов"),
		ERROR_ENTRY(117, "Ошибка при добавлении идентификатора в таблицу: таблица идентификаторов заполнена"),
		ERROR_ENTRY(118, "Ошибка при получении записи из таблицы идентификаторов: недопустимый номер идентификатора"),
		ERROR_ENTRY(119, "Не удалось определить лексему"),
		ERROR_ENTRY(120, "Длина строки превышает максимально допустимую длину (255)"),
		ERROR_ENTRY(121, "Длина идентификатора превышает максимально допустимую длину (32)"),
		ERROR_ENTRY(122, "Не закрыт строковый литерал"),
		ERROR_ENTRY_NODEF(123), ERROR_ENTRY_NODEF(124), ERROR_ENTRY_NODEF(125),
		ERROR_ENTRY_NODEF(126), ERROR_ENTRY_NODEF(127), ERROR_ENTRY_NODEF(128), ERROR_ENTRY_NODEF(129),
		ERROR_ENTRY_NODEF10(130),ERROR_ENTRY_NODEF10(140),ERROR_ENTRY_NODEF10(150),ERROR_ENTRY_NODEF10(160),
		ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),
		ERROR_ENTRY_NODEF100(200), 		
		ERROR_ENTRY(300, "Идентификатор не объявлен"),
		ERROR_ENTRY(301, "Повторное объявление идентификатора"),
		ERROR_ENTRY(302, "Имя совпадает с именем другого блока или переменной"),
		ERROR_ENTRY(303, "Имя совпадает с именем функции стандартной библиотеки"),
		ERROR_ENTRY(304, "Тип выражения не соответствует типу переменной"),
		ERROR_ENTRY(305, "Операнды арифметической операции должны иметь одинаковый тип number или short"),
		ERROR_ENTRY(306, "Операнд типа text недопустим в арифметической операции"),
		ERROR_ENTRY(307, "Целочисленный литерал выходит за диапазон типа"),
		ERROR_ENTRY(308, "Неверное количество аргументов при вызове блока"),
		ERROR_ENTRY(309, "Тип аргумента не соответствует типу параметра"),
		ERROR_ENTRY(310, "Вызов блока, не определённого ранее"),
		ERROR_ENTRY(311, "Процедура не может использоваться в выражении"),
		ERROR_ENTRY(312, "Тип выражения ret не соответствует возвращаемому типу блока"),
		ERROR_ENTRY(313, "Недопустимое преобразование типов"),
		ERROR_ENTRY(314, "Константа выходит за диапазон типа short при преобразовании"),
		ERROR_ENTRY(315, "Деление на нулевую константу"),
		ERROR_ENTRY(316, "Число повторений цикла должно иметь тип number или short"),
		ERROR_ENTRY(317, "Число повторений цикла является отрицательной константой"),
		ERROR_ENTRY(318, "В инструкции capture должна быть указана переменная"),
		ERROR_ENTRY(319, "Счётчик цикла доступен только для чтения"),
		ERROR_ENTRY(320, "Не определена точка входа main"),
		ERROR_ENTRY(321, "Определено более одной точки входа main"),
		ERROR_ENTRY(322, "Точка входа main должна возвращать значение типа number"),
		ERROR_ENTRY(323, "Унарная операция допустима только для минуса"), ERROR_ENTRY_NODEF(324), ERROR_ENTRY_NODEF(325), ERROR_ENTRY_NODEF(326),
		ERROR_ENTRY_NODEF(327), ERROR_ENTRY_NODEF(328), ERROR_ENTRY_NODEF(329),
		ERROR_ENTRY_NODEF10(330), ERROR_ENTRY_NODEF10(340), ERROR_ENTRY_NODEF10(350), ERROR_ENTRY_NODEF10(360),
		ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),
		ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),
		ERROR_ENTRY(600, "Неверная структура программы"),
		ERROR_ENTRY(601, "Ошибочный оператор"),
		ERROR_ENTRY(602, "Ошибка в выражении"),
		ERROR_ENTRY(603, "Ошибка в параметрах функции"),
		ERROR_ENTRY(604, "Ошибка в параметрах вызываемой функции"),
		ERROR_ENTRY(605, "Ошибка в продолжении выражения"),
		ERROR_ENTRY(606, "Функция должна завершаться ret"),
		ERROR_ENTRY(607, "Ошибка в теле процедуры"),
		ERROR_ENTRY_NODEF(608),
		ERROR_ENTRY_NODEF(609),
		ERROR_ENTRY_NODEF10(610), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
		ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680),
		ERROR_ENTRY_NODEF10(690),
		ERROR_ENTRY_NODEF100(700),
		ERROR_ENTRY(800, "Переменная объявлена, но не используется"),
		ERROR_ENTRY(801, "Значение переменной присваивается, но не используется"),
		ERROR_ENTRY(802, "Результат вызова функции не используется"),
		ERROR_ENTRY_NODEF(803), ERROR_ENTRY_NODEF(804), ERROR_ENTRY_NODEF(805), ERROR_ENTRY_NODEF(806),
		ERROR_ENTRY_NODEF(807), ERROR_ENTRY_NODEF(808), ERROR_ENTRY_NODEF(809),
		ERROR_ENTRY_NODEF10(810), ERROR_ENTRY_NODEF10(820), ERROR_ENTRY_NODEF10(830), ERROR_ENTRY_NODEF10(840),
		ERROR_ENTRY_NODEF10(850), ERROR_ENTRY_NODEF10(860), ERROR_ENTRY_NODEF10(870), ERROR_ENTRY_NODEF10(880),
		ERROR_ENTRY_NODEF10(890),
		ERROR_ENTRY(900, "Деление на ноль"),
		ERROR_ENTRY_NODEF(901), ERROR_ENTRY_NODEF(902), ERROR_ENTRY_NODEF(903), ERROR_ENTRY_NODEF(904),
		ERROR_ENTRY_NODEF(905), ERROR_ENTRY_NODEF(906), ERROR_ENTRY_NODEF(907), ERROR_ENTRY_NODEF(908),
		ERROR_ENTRY_NODEF(909),
		ERROR_ENTRY_NODEF10(910), ERROR_ENTRY_NODEF10(920), ERROR_ENTRY_NODEF10(930), ERROR_ENTRY_NODEF10(940),
		ERROR_ENTRY_NODEF10(950), ERROR_ENTRY_NODEF10(960), ERROR_ENTRY_NODEF10(970), ERROR_ENTRY_NODEF10(980),
		ERROR_ENTRY_NODEF10(990)
	};
	ERROR geterror(int id) {
		ERROR err;
		if (id < 0 || id >= ERROR_MAX_ENTRY) {
			err = errors[0];
		}
		else {
			err = errors[id];
		}
		return err;
	}
	ERROR geterrorin(int id, int line = -1, int col = -1) {
		ERROR err = geterror(id);
		err.inext = { (short)line,(short)col };
		return err;
	}
}
#define _CRT_SECURE_NO_WARNINGS
#include "Lexer.h"
#include "FST.h"
#include <string>
#include <vector>
#include "Error.h"
#include <iostream>
#include <iomanip>

#define _NUMBER "number"
#define _TEXT "text"
#define _VOID "void"
#define _SMNUMBER "smlnumber"

namespace LEX {
	FST::FST typeNumber((char*)"", 7,
		FST::NODE(1, FST::RELATION('n', 1)),
		FST::NODE(1, FST::RELATION('u', 2)),
		FST::NODE(1, FST::RELATION('m', 3)),
		FST::NODE(1, FST::RELATION('b', 4)),
		FST::NODE(1, FST::RELATION('e', 5)),
		FST::NODE(1, FST::RELATION('r', 6)),
		FST::NODE());

	FST::FST typeSmallNumber((char*)"",10,
		FST::NODE(1, FST::RELATION('s', 1)),
		FST::NODE(1, FST::RELATION('m', 2)),
		FST::NODE(1, FST::RELATION('l', 3)),
		FST::NODE(1, FST::RELATION('n', 4)),
		FST::NODE(1, FST::RELATION('u', 5)),
		FST::NODE(1, FST::RELATION('m', 6)),
		FST::NODE(1, FST::RELATION('b', 7)),
		FST::NODE(1, FST::RELATION('e', 8)),
		FST::NODE(1, FST::RELATION('r', 9)),
		FST::NODE());

	FST::FST typeText((char*)"", 5,
		FST::NODE(1, FST::RELATION('t', 1)),
		FST::NODE(1, FST::RELATION('e', 2)),
		FST::NODE(1, FST::RELATION('x', 3)),
		FST::NODE(1, FST::RELATION('t', 4)),
		FST::NODE());

	FST::FST typeVoid((char*)"", 5,
		FST::NODE(1, FST::RELATION('v', 1)),
		FST::NODE(1, FST::RELATION('o', 2)),
		FST::NODE(1, FST::RELATION('i', 3)),
		FST::NODE(1, FST::RELATION('d', 4)),
		FST::NODE());

	FST::FST typeFunction((char*)"", 3,
		FST::NODE(1, FST::RELATION('f', 1)),
		FST::NODE(1, FST::RELATION('n', 2)),
		FST::NODE());

	FST::FST typeReturn((char*)"", 4,
		FST::NODE(1, FST::RELATION('r', 1)),
		FST::NODE(1, FST::RELATION('e', 2)),
		FST::NODE(1, FST::RELATION('t', 3)),
		FST::NODE());

	FST::FST typeProject((char*)"", 8,
		FST::NODE(1, FST::RELATION('p', 1)),
		FST::NODE(1, FST::RELATION('r', 2)),
		FST::NODE(1, FST::RELATION('o', 3)),
		FST::NODE(1, FST::RELATION('j', 4)),
		FST::NODE(1, FST::RELATION('e', 5)),
		FST::NODE(1, FST::RELATION('c', 6)),
		FST::NODE(1, FST::RELATION('t', 7)),
		FST::NODE());

	FST::FST typeCapture((char*)"", 8,
		FST::NODE(1, FST::RELATION('c', 1)),
		FST::NODE(1, FST::RELATION('a', 2)),
		FST::NODE(1, FST::RELATION('p', 3)),
		FST::NODE(1, FST::RELATION('t', 4)),
		FST::NODE(1, FST::RELATION('u', 5)),
		FST::NODE(1, FST::RELATION('r', 6)),
		FST::NODE(1, FST::RELATION('e', 7)),
		FST::NODE());


	FST::FST typeRepeat((char*)"", 7,
		FST::NODE(1, FST::RELATION('r', 1)),
		FST::NODE(1, FST::RELATION('e', 2)),
		FST::NODE(1, FST::RELATION('p', 3)),
		FST::NODE(1, FST::RELATION('e', 4)),
		FST::NODE(1, FST::RELATION('a', 5)),
		FST::NODE(1, FST::RELATION('t', 6)),
		FST::NODE());

	FST::FST typeMain((char*)"", 5,
		FST::NODE(1, FST::RELATION('m', 1)),
		FST::NODE(1, FST::RELATION('a', 2)),
		FST::NODE(1, FST::RELATION('i', 3)),
		FST::NODE(1, FST::RELATION('n', 4)),
		FST::NODE());

	FST::FST typeSemicolon((char*)"", 2,
		FST::NODE(1, FST::RELATION(';', 1)),
		FST::NODE());

	FST::FST typeComma((char*)"", 2,
		FST::NODE(1, FST::RELATION(',', 1)),
		FST::NODE());

	FST::FST typeLeftThesis((char*)"", 2,
		FST::NODE(1, FST::RELATION('(', 1)),
		FST::NODE());

	FST::FST typeRightThesis((char*)"", 2,
		FST::NODE(1, FST::RELATION(')', 1)),
		FST::NODE());

	FST::FST typeEquals((char*)"", 2,
		FST::NODE(1, FST::RELATION('=', 1)),
		FST::NODE());

	FST::FST typePlus((char*)"", 2,
		FST::NODE(1, FST::RELATION('+', 1)),
		FST::NODE());

	FST::FST typeMinus((char*)"", 2,
		FST::NODE(1, FST::RELATION('-', 1)),
		FST::NODE());

	FST::FST typeMulti((char*)"", 2,
		FST::NODE(1, FST::RELATION('*', 1)),
		FST::NODE());

	FST::FST typeDiv((char*)"", 2,
		FST::NODE(1, FST::RELATION('/', 1)),
		FST::NODE());

	FST::FST typeMod((char*)"", 2,
		FST::NODE(1, FST::RELATION('#', 1)),
		FST::NODE());

	FST::FST typeColon((char*)"", 2,
		FST::NODE(1, FST::RELATION(':', 1)),
		FST::NODE());

	FST::FST typeArrow((char*)"", 3,
		FST::NODE(1, FST::RELATION('-', 1)),
		FST::NODE(1, FST::RELATION('>', 2)),
		FST::NODE());

	FST::FST typeAssign((char*)"", 3,
		FST::NODE(1, FST::RELATION(':', 1)),
		FST::NODE(1, FST::RELATION('=', 2)),
		FST::NODE());

	FST::FST typeAs((char*)"", 3,
		FST::NODE(1, FST::RELATION('a', 1)),
		FST::NODE(1, FST::RELATION('s', 2)),
		FST::NODE());

	FST::FST typeEnd((char*)"", 4,
		FST::NODE(1, FST::RELATION('e', 1)),
		FST::NODE(1, FST::RELATION('n', 2)),
		FST::NODE(1, FST::RELATION('d', 3)),
		FST::NODE());

	FST::FST typeIdentificator((char*)"", 3,
		FST::NODE(238,
			FST::RELATION('a', 1), FST::RELATION('b', 1), FST::RELATION('c', 1), FST::RELATION('d', 1),
			FST::RELATION('e', 1), FST::RELATION('f', 1), FST::RELATION('g', 1), FST::RELATION('h', 1),
			FST::RELATION('i', 1), FST::RELATION('j', 1), FST::RELATION('k', 1), FST::RELATION('l', 1),
			FST::RELATION('m', 1), FST::RELATION('n', 1), FST::RELATION('o', 1), FST::RELATION('p', 1),
			FST::RELATION('q', 1), FST::RELATION('r', 1), FST::RELATION('s', 1), FST::RELATION('t', 1),
			FST::RELATION('u', 1), FST::RELATION('v', 1), FST::RELATION('w', 1), FST::RELATION('x', 1),
			FST::RELATION('y', 1), FST::RELATION('z', 1), FST::RELATION('A', 1), FST::RELATION('B', 1),
			FST::RELATION('C', 1), FST::RELATION('D', 1), FST::RELATION('E', 1), FST::RELATION('F', 1),
			FST::RELATION('G', 1), FST::RELATION('H', 1), FST::RELATION('I', 1), FST::RELATION('J', 1),
			FST::RELATION('K', 1), FST::RELATION('L', 1), FST::RELATION('M', 1), FST::RELATION('N', 1),
			FST::RELATION('O', 1), FST::RELATION('P', 1), FST::RELATION('Q', 1), FST::RELATION('R', 1),
			FST::RELATION('S', 1), FST::RELATION('T', 1), FST::RELATION('U', 1), FST::RELATION('V', 1),
			FST::RELATION('W', 1), FST::RELATION('X', 1), FST::RELATION('Y', 1), FST::RELATION('Z', 1),
			FST::RELATION('_', 1),
			FST::RELATION('À', 1), FST::RELATION('Á', 1), FST::RELATION('Â', 1), FST::RELATION('Ã', 1),
			FST::RELATION('Ä', 1), FST::RELATION('Å', 1), FST::RELATION('Æ', 1), FST::RELATION('Ç', 1),
			FST::RELATION('È', 1), FST::RELATION('É', 1), FST::RELATION('Ê', 1), FST::RELATION('Ë', 1),
			FST::RELATION('Ì', 1), FST::RELATION('Í', 1), FST::RELATION('Î', 1), FST::RELATION('Ï', 1),
			FST::RELATION('Ð', 1), FST::RELATION('Ñ', 1), FST::RELATION('Ò', 1), FST::RELATION('Ó', 1),
			FST::RELATION('Ô', 1), FST::RELATION('Õ', 1), FST::RELATION('Ö', 1), FST::RELATION('×', 1),
			FST::RELATION('Ø', 1), FST::RELATION('Ù', 1), FST::RELATION('Ú', 1), FST::RELATION('Û', 1),
			FST::RELATION('Ü', 1), FST::RELATION('Ý', 1), FST::RELATION('Þ', 1), FST::RELATION('ß', 1),
			FST::RELATION('¨', 1), FST::RELATION('à', 1), FST::RELATION('á', 1), FST::RELATION('â', 1),
			FST::RELATION('ã', 1), FST::RELATION('ä', 1), FST::RELATION('å', 1), FST::RELATION('æ', 1),
			FST::RELATION('ç', 1), FST::RELATION('è', 1), FST::RELATION('é', 1), FST::RELATION('ê', 1),
			FST::RELATION('ë', 1), FST::RELATION('ì', 1), FST::RELATION('í', 1), FST::RELATION('î', 1),
			FST::RELATION('ï', 1), FST::RELATION('ð', 1), FST::RELATION('ñ', 1), FST::RELATION('ò', 1),
			FST::RELATION('ó', 1), FST::RELATION('ô', 1), FST::RELATION('õ', 1), FST::RELATION('ö', 1),
			FST::RELATION('÷', 1), FST::RELATION('ø', 1), FST::RELATION('ù', 1), FST::RELATION('ú', 1),
			FST::RELATION('û', 1), FST::RELATION('ü', 1), FST::RELATION('ý', 1), FST::RELATION('þ', 1),
			FST::RELATION('ÿ', 1), FST::RELATION('¸', 1),
			FST::RELATION('a', 2), FST::RELATION('b', 2), FST::RELATION('c', 2), FST::RELATION('d', 2),
			FST::RELATION('e', 2), FST::RELATION('f', 2), FST::RELATION('g', 2), FST::RELATION('h', 2),
			FST::RELATION('i', 2), FST::RELATION('j', 2), FST::RELATION('k', 2), FST::RELATION('l', 2),
			FST::RELATION('m', 2), FST::RELATION('n', 2), FST::RELATION('o', 2), FST::RELATION('p', 2),
			FST::RELATION('q', 2), FST::RELATION('r', 2), FST::RELATION('s', 2), FST::RELATION('t', 2),
			FST::RELATION('u', 2), FST::RELATION('v', 2), FST::RELATION('w', 2), FST::RELATION('x', 2),
			FST::RELATION('y', 2), FST::RELATION('z', 2), FST::RELATION('A', 2), FST::RELATION('B', 2),
			FST::RELATION('C', 2), FST::RELATION('D', 2), FST::RELATION('E', 2), FST::RELATION('F', 2),
			FST::RELATION('G', 2), FST::RELATION('H', 2), FST::RELATION('I', 2), FST::RELATION('J', 2),
			FST::RELATION('K', 2), FST::RELATION('L', 2), FST::RELATION('M', 2), FST::RELATION('N', 2),
			FST::RELATION('O', 2), FST::RELATION('P', 2), FST::RELATION('Q', 2), FST::RELATION('R', 2),
			FST::RELATION('S', 2), FST::RELATION('T', 2), FST::RELATION('U', 2), FST::RELATION('V', 2),
			FST::RELATION('W', 2), FST::RELATION('X', 2), FST::RELATION('Y', 2), FST::RELATION('Z', 2),
			FST::RELATION('_', 2), FST::RELATION('À', 2), FST::RELATION('Á', 2), FST::RELATION('Â', 2), FST::RELATION('Ã', 2),
			FST::RELATION('Ä', 2), FST::RELATION('Å', 2), FST::RELATION('Æ', 2), FST::RELATION('Ç', 2),
			FST::RELATION('È', 2), FST::RELATION('É', 2), FST::RELATION('Ê', 2), FST::RELATION('Ë', 2),
			FST::RELATION('Ì', 2), FST::RELATION('Í', 2), FST::RELATION('Î', 2), FST::RELATION('Ï', 2),
			FST::RELATION('Ð', 2), FST::RELATION('Ñ', 2), FST::RELATION('Ò', 2), FST::RELATION('Ó', 2),
			FST::RELATION('Ô', 2), FST::RELATION('Õ', 2), FST::RELATION('Ö', 2), FST::RELATION('×', 2),
			FST::RELATION('Ø', 2), FST::RELATION('Ù', 2), FST::RELATION('Ú', 2), FST::RELATION('Û', 2),
			FST::RELATION('Ü', 2), FST::RELATION('Ý', 2), FST::RELATION('Þ', 2), FST::RELATION('ß', 2),
			FST::RELATION('¨', 2), FST::RELATION('à', 2), FST::RELATION('á', 2), FST::RELATION('â', 2),
			FST::RELATION('ã', 2), FST::RELATION('ä', 2), FST::RELATION('å', 2), FST::RELATION('æ', 2),
			FST::RELATION('ç', 2), FST::RELATION('è', 2), FST::RELATION('é', 2), FST::RELATION('ê', 2),
			FST::RELATION('ë', 2), FST::RELATION('ì', 2), FST::RELATION('í', 2), FST::RELATION('î', 2),
			FST::RELATION('ï', 2), FST::RELATION('ð', 2), FST::RELATION('ñ', 2), FST::RELATION('ò', 2),
			FST::RELATION('ó', 2), FST::RELATION('ô', 2), FST::RELATION('õ', 2), FST::RELATION('ö', 2),
			FST::RELATION('÷', 2), FST::RELATION('ø', 2), FST::RELATION('ù', 2), FST::RELATION('ú', 2),
			FST::RELATION('û', 2), FST::RELATION('ü', 2), FST::RELATION('ý', 2), FST::RELATION('þ', 2),
			FST::RELATION('ÿ', 2), FST::RELATION('¸', 2)),
	    FST::NODE(258,
			FST::RELATION('a', 1), FST::RELATION('b', 1), FST::RELATION('c', 1), FST::RELATION('d', 1),
			FST::RELATION('e', 1), FST::RELATION('f', 1), FST::RELATION('g', 1), FST::RELATION('h', 1),
			FST::RELATION('i', 1), FST::RELATION('j', 1), FST::RELATION('k', 1), FST::RELATION('l', 1),
			FST::RELATION('m', 1), FST::RELATION('n', 1), FST::RELATION('o', 1), FST::RELATION('p', 1),
			FST::RELATION('q', 1), FST::RELATION('r', 1), FST::RELATION('s', 1), FST::RELATION('t', 1),
			FST::RELATION('u', 1), FST::RELATION('v', 1), FST::RELATION('w', 1), FST::RELATION('x', 1),
			FST::RELATION('y', 1), FST::RELATION('z', 1), FST::RELATION('A', 1), FST::RELATION('B', 1),
			FST::RELATION('C', 1), FST::RELATION('D', 1), FST::RELATION('E', 1), FST::RELATION('F', 1),
			FST::RELATION('G', 1), FST::RELATION('H', 1), FST::RELATION('I', 1), FST::RELATION('J', 1),
			FST::RELATION('K', 1), FST::RELATION('L', 1), FST::RELATION('M', 1), FST::RELATION('N', 1),
			FST::RELATION('O', 1), FST::RELATION('P', 1), FST::RELATION('Q', 1), FST::RELATION('R', 1),
			FST::RELATION('S', 1), FST::RELATION('T', 1), FST::RELATION('U', 1), FST::RELATION('V', 1),
			FST::RELATION('W', 1), FST::RELATION('X', 1), FST::RELATION('Y', 1), FST::RELATION('Z', 1),
			FST::RELATION('_', 1), FST::RELATION('À', 1), FST::RELATION('Á', 1), FST::RELATION('Â', 1), FST::RELATION('Ã', 1),
			FST::RELATION('Ä', 1), FST::RELATION('Å', 1), FST::RELATION('Æ', 1), FST::RELATION('Ç', 1),
			FST::RELATION('È', 1), FST::RELATION('É', 1), FST::RELATION('Ê', 1), FST::RELATION('Ë', 1),
			FST::RELATION('Ì', 1), FST::RELATION('Í', 1), FST::RELATION('Î', 1), FST::RELATION('Ï', 1),
			FST::RELATION('Ð', 1), FST::RELATION('Ñ', 1), FST::RELATION('Ò', 1), FST::RELATION('Ó', 1),
			FST::RELATION('Ô', 1), FST::RELATION('Õ', 1), FST::RELATION('Ö', 1), FST::RELATION('×', 1),
			FST::RELATION('Ø', 1), FST::RELATION('Ù', 1), FST::RELATION('Ú', 1), FST::RELATION('Û', 1),
			FST::RELATION('Ü', 1), FST::RELATION('Ý', 1), FST::RELATION('Þ', 1), FST::RELATION('ß', 1),
			FST::RELATION('¨', 1), FST::RELATION('à', 1), FST::RELATION('á', 1), FST::RELATION('â', 1),
			FST::RELATION('ã', 1), FST::RELATION('ä', 1), FST::RELATION('å', 1), FST::RELATION('æ', 1),
			FST::RELATION('ç', 1), FST::RELATION('è', 1), FST::RELATION('é', 1), FST::RELATION('ê', 1),
			FST::RELATION('ë', 1), FST::RELATION('ì', 1), FST::RELATION('í', 1), FST::RELATION('î', 1),
			FST::RELATION('ï', 1), FST::RELATION('ð', 1), FST::RELATION('ñ', 1), FST::RELATION('ò', 1),
			FST::RELATION('ó', 1), FST::RELATION('ô', 1), FST::RELATION('õ', 1), FST::RELATION('ö', 1),
			FST::RELATION('÷', 1), FST::RELATION('ø', 1), FST::RELATION('ù', 1), FST::RELATION('ú', 1),
			FST::RELATION('û', 1), FST::RELATION('ü', 1), FST::RELATION('ý', 1), FST::RELATION('þ', 1),
			FST::RELATION('ÿ', 1), FST::RELATION('¸', 1),
			FST::RELATION('À', 2), FST::RELATION('Á', 2), FST::RELATION('Â', 2), FST::RELATION('Ã', 2),
			FST::RELATION('Ä', 2), FST::RELATION('Å', 2), FST::RELATION('Æ', 2), FST::RELATION('Ç', 2),
			FST::RELATION('È', 2), FST::RELATION('É', 2), FST::RELATION('Ê', 2), FST::RELATION('Ë', 2),
			FST::RELATION('Ì', 2), FST::RELATION('Í', 2), FST::RELATION('Î', 2), FST::RELATION('Ï', 2),
			FST::RELATION('Ð', 2), FST::RELATION('Ñ', 2), FST::RELATION('Ò', 2), FST::RELATION('Ó', 2),
			FST::RELATION('Ô', 2), FST::RELATION('Õ', 2), FST::RELATION('Ö', 2), FST::RELATION('×', 2),
			FST::RELATION('Ø', 2), FST::RELATION('Ù', 2), FST::RELATION('Ú', 2), FST::RELATION('Û', 2),
			FST::RELATION('Ü', 2), FST::RELATION('Ý', 2), FST::RELATION('Þ', 2), FST::RELATION('ß', 2),
			FST::RELATION('¨', 2), FST::RELATION('à', 2), FST::RELATION('á', 2), FST::RELATION('â', 2),
			FST::RELATION('ã', 2), FST::RELATION('ä', 2), FST::RELATION('å', 2), FST::RELATION('æ', 2),
			FST::RELATION('ç', 2), FST::RELATION('è', 2), FST::RELATION('é', 2), FST::RELATION('ê', 2),
			FST::RELATION('ë', 2), FST::RELATION('ì', 2), FST::RELATION('í', 2), FST::RELATION('î', 2),
			FST::RELATION('ï', 2), FST::RELATION('ð', 2), FST::RELATION('ñ', 2), FST::RELATION('ò', 2),
			FST::RELATION('ó', 2), FST::RELATION('ô', 2), FST::RELATION('õ', 2), FST::RELATION('ö', 2),
			FST::RELATION('÷', 2), FST::RELATION('ø', 2), FST::RELATION('ù', 2), FST::RELATION('ú', 2),
			FST::RELATION('û', 2), FST::RELATION('ü', 2), FST::RELATION('ý', 2), FST::RELATION('þ', 2),
			FST::RELATION('ÿ', 2), FST::RELATION('¸', 2),
			FST::RELATION('0', 1), FST::RELATION('1', 1), FST::RELATION('2', 1), FST::RELATION('3', 1),
			FST::RELATION('4', 1), FST::RELATION('5', 1), FST::RELATION('6', 1), FST::RELATION('7', 1),
			FST::RELATION('8', 1), FST::RELATION('9', 1),
			FST::RELATION('a', 2), FST::RELATION('b', 2), FST::RELATION('c', 2), FST::RELATION('d', 2),
			FST::RELATION('e', 2), FST::RELATION('f', 2), FST::RELATION('g', 2), FST::RELATION('h', 2),
			FST::RELATION('i', 2), FST::RELATION('j', 2), FST::RELATION('k', 2), FST::RELATION('l', 2),
			FST::RELATION('m', 2), FST::RELATION('n', 2), FST::RELATION('o', 2), FST::RELATION('p', 2),
			FST::RELATION('q', 2), FST::RELATION('r', 2), FST::RELATION('s', 2), FST::RELATION('t', 2),
			FST::RELATION('u', 2), FST::RELATION('v', 2), FST::RELATION('w', 2), FST::RELATION('x', 2),
			FST::RELATION('y', 2), FST::RELATION('z', 2), FST::RELATION('A', 2), FST::RELATION('B', 2),
			FST::RELATION('C', 2), FST::RELATION('D', 2), FST::RELATION('E', 2), FST::RELATION('F', 2),
			FST::RELATION('G', 2), FST::RELATION('H', 2), FST::RELATION('I', 2), FST::RELATION('J', 2),
			FST::RELATION('K', 2), FST::RELATION('L', 2), FST::RELATION('M', 2), FST::RELATION('N', 2),
			FST::RELATION('O', 2), FST::RELATION('P', 2), FST::RELATION('Q', 2), FST::RELATION('R', 2),
			FST::RELATION('S', 2), FST::RELATION('T', 2), FST::RELATION('U', 2), FST::RELATION('V', 2),
			FST::RELATION('W', 2), FST::RELATION('X', 2), FST::RELATION('Y', 2), FST::RELATION('Z', 2),
			FST::RELATION('_', 2),
			FST::RELATION('0', 2), FST::RELATION('1', 2), FST::RELATION('2', 2), FST::RELATION('3', 2),
			FST::RELATION('4', 2), FST::RELATION('5', 2), FST::RELATION('6', 2), FST::RELATION('7', 2),
			FST::RELATION('8', 2), FST::RELATION('9', 2)),
			FST::NODE());

	FST::FST typeStringLiteral((char*)"", 4,
		FST::NODE(2,
			FST::RELATION('\'', 1), FST::RELATION('\"', 2)),
		FST::NODE(159,
			FST::RELATION('a', 1), FST::RELATION('b', 1), FST::RELATION('c', 1), FST::RELATION('d', 1), FST::RELATION('e', 1), FST::RELATION('f', 1), FST::RELATION('g', 1), FST::RELATION('h', 1),
			FST::RELATION('i', 1), FST::RELATION('j', 1), FST::RELATION('k', 1), FST::RELATION('l', 1), FST::RELATION('m', 1), FST::RELATION('n', 1), FST::RELATION('o', 1), FST::RELATION('p', 1),
			FST::RELATION('q', 1), FST::RELATION('r', 1), FST::RELATION('s', 1), FST::RELATION('t', 1), FST::RELATION('u', 1), FST::RELATION('v', 1), FST::RELATION('w', 1), FST::RELATION('x', 1),
			FST::RELATION('y', 1), FST::RELATION('z', 1), FST::RELATION('A', 1), FST::RELATION('B', 1), FST::RELATION('C', 1), FST::RELATION('D', 1), FST::RELATION('E', 1), FST::RELATION('F', 1),
			FST::RELATION('G', 1), FST::RELATION('H', 1), FST::RELATION('I', 1), FST::RELATION('J', 1), FST::RELATION('K', 1), FST::RELATION('L', 1), FST::RELATION('M', 1), FST::RELATION('N', 1),
			FST::RELATION('O', 1), FST::RELATION('P', 1), FST::RELATION('Q', 1), FST::RELATION('R', 1), FST::RELATION('S', 1), FST::RELATION('T', 1), FST::RELATION('U', 1), FST::RELATION('V', 1),
			FST::RELATION('W', 1), FST::RELATION('X', 1), FST::RELATION('Y', 1), FST::RELATION('Z', 1), FST::RELATION('¸', 1), FST::RELATION('¨', 1), FST::RELATION('À', 1), FST::RELATION('Á', 1),
			FST::RELATION('Â', 1), FST::RELATION('Ã', 1), FST::RELATION('Ä', 1), FST::RELATION('Å', 1), FST::RELATION('Æ', 1), FST::RELATION('Ç', 1), FST::RELATION('È', 1), FST::RELATION('É', 1),
			FST::RELATION('Ê', 1), FST::RELATION('Ë', 1), FST::RELATION('Ì', 1), FST::RELATION('Í', 1), FST::RELATION('Î', 1), FST::RELATION('Ï', 1), FST::RELATION('Ð', 1), FST::RELATION('Ñ', 1),
			FST::RELATION('Ò', 1), FST::RELATION('Ó', 1), FST::RELATION('Ô', 1), FST::RELATION('Õ', 1), FST::RELATION('Ö', 1), FST::RELATION('×', 1), FST::RELATION('Ø', 1), FST::RELATION('Ù', 1),
			FST::RELATION('Ú', 1), FST::RELATION('Û', 1), FST::RELATION('Ü', 1), FST::RELATION('Ý', 1), FST::RELATION('Þ', 1), FST::RELATION('ß', 1), FST::RELATION('à', 1), FST::RELATION('á', 1),
			FST::RELATION('â', 1), FST::RELATION('ã', 1), FST::RELATION('ä', 1), FST::RELATION('å', 1), FST::RELATION('æ', 1), FST::RELATION('ç', 1), FST::RELATION('è', 1), FST::RELATION('é', 1),
			FST::RELATION('ê', 1), FST::RELATION('ë', 1), FST::RELATION('ì', 1), FST::RELATION('í', 1), FST::RELATION('î', 1), FST::RELATION('ï', 1), FST::RELATION('ð', 1), FST::RELATION('ñ', 1),
			FST::RELATION('ò', 1), FST::RELATION('ó', 1), FST::RELATION('ô', 1), FST::RELATION('õ', 1), FST::RELATION('ö', 1), FST::RELATION('÷', 1), FST::RELATION('ø', 1), FST::RELATION('ù', 1),
			FST::RELATION('ú', 1), FST::RELATION('û', 1), FST::RELATION('ü', 1), FST::RELATION('ý', 1), FST::RELATION('þ', 1), FST::RELATION('ÿ', 1), FST::RELATION('0', 1), FST::RELATION('1', 1),
			FST::RELATION('2', 1), FST::RELATION('3', 1), FST::RELATION('4', 1), FST::RELATION('5', 1), FST::RELATION('6', 1), FST::RELATION('7', 1), FST::RELATION('8', 1), FST::RELATION('9', 1),
			FST::RELATION('!', 1), FST::RELATION('@', 1), FST::RELATION('#', 1), FST::RELATION('$', 1), FST::RELATION('%', 1), FST::RELATION('^', 1), FST::RELATION('&', 1), FST::RELATION('*', 1),
			FST::RELATION('(', 1), FST::RELATION(')', 1), FST::RELATION('-', 1), FST::RELATION('_', 1), FST::RELATION('+', 1), FST::RELATION('=', 1), FST::RELATION('~', 1), FST::RELATION('`', 1),
			FST::RELATION('{', 1), FST::RELATION('}', 1), FST::RELATION('\\', 1), FST::RELATION('|', 1), FST::RELATION(':', 1), FST::RELATION(';', 1), FST::RELATION(',', 1), FST::RELATION('/', 1),
			FST::RELATION('<', 1), FST::RELATION('>', 1), FST::RELATION('?', 1), FST::RELATION(' ', 1), FST::RELATION('\t', 1), FST::RELATION('"', 1),
			FST::RELATION('\'', 3)),
		FST::NODE(159,
			FST::RELATION('a', 2), FST::RELATION('b', 2), FST::RELATION('c', 2), FST::RELATION('d', 2), FST::RELATION('e', 2), FST::RELATION('f', 2), FST::RELATION('g', 2), FST::RELATION('h', 2),
			FST::RELATION('i', 2), FST::RELATION('j', 2), FST::RELATION('k', 2), FST::RELATION('l', 2), FST::RELATION('m', 2), FST::RELATION('n', 2), FST::RELATION('o', 2), FST::RELATION('p', 2),
			FST::RELATION('q', 2), FST::RELATION('r', 2), FST::RELATION('s', 2), FST::RELATION('t', 2), FST::RELATION('u', 2), FST::RELATION('v', 2), FST::RELATION('w', 2), FST::RELATION('x', 2),
			FST::RELATION('y', 2), FST::RELATION('z', 2), FST::RELATION('A', 2), FST::RELATION('B', 2), FST::RELATION('C', 2), FST::RELATION('D', 2), FST::RELATION('E', 2), FST::RELATION('F', 2),
			FST::RELATION('G', 2), FST::RELATION('H', 2), FST::RELATION('I', 2), FST::RELATION('J', 2), FST::RELATION('K', 2), FST::RELATION('L', 2), FST::RELATION('M', 2), FST::RELATION('N', 2),
			FST::RELATION('O', 2), FST::RELATION('P', 2), FST::RELATION('Q', 2), FST::RELATION('R', 2), FST::RELATION('S', 2), FST::RELATION('T', 2), FST::RELATION('U', 2), FST::RELATION('V', 2),
			FST::RELATION('W', 2), FST::RELATION('X', 2), FST::RELATION('Y', 2), FST::RELATION('Z', 2), FST::RELATION('¸', 2), FST::RELATION('¨', 2), FST::RELATION('À', 2), FST::RELATION('Á', 2),
			FST::RELATION('Â', 2), FST::RELATION('Ã', 2), FST::RELATION('Ä', 2), FST::RELATION('Å', 2), FST::RELATION('Æ', 2), FST::RELATION('Ç', 2), FST::RELATION('È', 2), FST::RELATION('É', 2),
			FST::RELATION('Ê', 2), FST::RELATION('Ë', 2), FST::RELATION('Ì', 2), FST::RELATION('Í', 2), FST::RELATION('Î', 2), FST::RELATION('Ï', 2), FST::RELATION('Ð', 2), FST::RELATION('Ñ', 2),
			FST::RELATION('Ò', 2), FST::RELATION('Ó', 2), FST::RELATION('Ô', 2), FST::RELATION('Õ', 2), FST::RELATION('Ö', 2), FST::RELATION('×', 2), FST::RELATION('Ø', 2), FST::RELATION('Ù', 2),
			FST::RELATION('Ú', 2), FST::RELATION('Û', 2), FST::RELATION('Ü', 2), FST::RELATION('Ý', 2), FST::RELATION('Þ', 2), FST::RELATION('ß', 2), FST::RELATION('à', 2), FST::RELATION('á', 2),
			FST::RELATION('â', 2), FST::RELATION('ã', 2), FST::RELATION('ä', 2), FST::RELATION('å', 2), FST::RELATION('æ', 2), FST::RELATION('ç', 2), FST::RELATION('è', 2), FST::RELATION('é', 2),
			FST::RELATION('ê', 2), FST::RELATION('ë', 2), FST::RELATION('ì', 2), FST::RELATION('í', 2), FST::RELATION('î', 2), FST::RELATION('ï', 2), FST::RELATION('ð', 2), FST::RELATION('ñ', 2),
			FST::RELATION('ò', 2), FST::RELATION('ó', 2), FST::RELATION('ô', 2), FST::RELATION('õ', 2), FST::RELATION('ö', 2), FST::RELATION('÷', 2), FST::RELATION('ø', 2), FST::RELATION('ù', 2),
			FST::RELATION('ú', 2), FST::RELATION('û', 2), FST::RELATION('ü', 2), FST::RELATION('ý', 2), FST::RELATION('þ', 2), FST::RELATION('ÿ', 2), FST::RELATION('0', 2), FST::RELATION('1', 2),
			FST::RELATION('2', 2), FST::RELATION('3', 2), FST::RELATION('4', 2), FST::RELATION('5', 2), FST::RELATION('6', 2), FST::RELATION('7', 2), FST::RELATION('8', 2), FST::RELATION('9', 2),
			FST::RELATION('!', 2), FST::RELATION('@', 2), FST::RELATION('#', 2), FST::RELATION('$', 2), FST::RELATION('%', 2), FST::RELATION('^', 2), FST::RELATION('&', 2), FST::RELATION('*', 2),
			FST::RELATION('(', 2), FST::RELATION(')', 2), FST::RELATION('-', 2), FST::RELATION('_', 2), FST::RELATION('+', 2), FST::RELATION('=', 2), FST::RELATION('~', 2), FST::RELATION('`', 2),
			FST::RELATION('{', 2), FST::RELATION('}', 2), FST::RELATION('\\', 2), FST::RELATION('|', 2), FST::RELATION(':', 2), FST::RELATION(';', 2), FST::RELATION(',', 2), FST::RELATION('/', 2),
			FST::RELATION('<', 2), FST::RELATION('>', 2), FST::RELATION('?', 2), FST::RELATION(' ', 2), FST::RELATION('\t', 2), FST::RELATION('\'', 2),
			FST::RELATION('\"', 3)),
		FST::NODE()
	);

	FST::FST typeDecNumber((char*)"", 2,
		FST::NODE(19, 
			FST::RELATION('1', 0), FST::RELATION('2', 0), 
			FST::RELATION('3', 0), FST::RELATION('4', 0), FST::RELATION('5', 0), 
			FST::RELATION('6', 0), FST::RELATION('7', 0), FST::RELATION('8', 0), 
			FST::RELATION('9', 0), 
			FST::RELATION('0', 1), FST::RELATION('1', 1), FST::RELATION('2', 1), 
			FST::RELATION('3', 1), FST::RELATION('4', 1), FST::RELATION('5', 1), 
			FST::RELATION('6', 1), FST::RELATION('7', 1), FST::RELATION('8', 1), 
			FST::RELATION('9', 1)),
		FST::NODE());

	FST::FST typeBinNumber((char*)"", 3,
		FST::NODE(2, FST::RELATION('0', 1), FST::RELATION('1', 1)),
		FST::NODE(3, FST::RELATION('0', 1), FST::RELATION('1', 1), FST::RELATION('b', 2)),
		FST::NODE());

	bool Check(std::string s, FST::FST fst)
	{
		fst.string = s;
		return FST::execute(fst);
	}
	std::vector<std::string> scopes = { "" };
	int loopCounter = 0;               
	std::string currentScope = "";
	bool inHeader = false;

	long long ParseInt(const std::string& s, bool& overflow) {
		int base = 10;
		std::string d = s;
		if (d.back() == 'b') { 
			base = 2; 
			d.pop_back(); 
		}
		long long value = 0;
		overflow = false;
		for (char c : d) {
			value = value * base + (c - '0');
			if (value > INT_MAX) { 
				overflow = true; 
				break; 
			}
		}
		return overflow ? INT_MAX : value;
	}


	int AddIDtoTable(IT::IdTable& it, LT::LexTable& lt, int ind, std::vector<In::WORD>& words) {
		std::string name = words[ind].str;
		LT::Entry prev = (lt.size > 0) ? LT::GetEntry(lt, lt.size - 1) : LT::Entry{};

		int idx;
		if (prev.lexema == LEX_FUNCTION) {
			idx = IT::IsId(it, name, "");
		}
		else {
			idx = IT::FindVisible(it, scopes, name);
		}

		if (idx == TI_NULLIDX) {
			int firstLex = lt.size;
			IT::Entry ent;
			switch (prev.lexema) {
			case LEX_FUNCTION: {
				ent = { firstLex, name, IT::IDDATATYPE::UNDEF, IT::IDTYPE::F, "" };
				break;
			}
			case LEX_COMMA:
			case LEX_LEFTHESIS: {
				if (inHeader) {
					ent = { firstLex, name, IT::IDDATATYPE::UNDEF, IT::IDTYPE::P, scopes.back() };
				}
				else {
					ent = { firstLex, name, IT::IDDATATYPE::UNDEF, IT::IDTYPE::V, scopes.back() };
				}
				break;
			}
			case LEX_AS: {
				ent = { firstLex, name, IT::IDDATATYPE::INT, IT::IDTYPE::V, scopes.back() };
				break;
			}
			default: {
				ent = { firstLex, name, IT::IDDATATYPE::UNDEF, IT::IDTYPE::V, scopes.back() };
				break;
			}
			}
			IT::Add(it, ent);
			idx = it.size - 1;
		}

		if (prev.lexema == LEX_FUNCTION) {
			scopes.push_back(name);
			inHeader = true;
		}
		return idx;
	}

	void SetType(IT::IdTable& it, LT::LexTable& lt, int ind, std::vector<In::WORD>& words) {
		if (lt.size < 3) return;
		int pos = lt.size - 3;
		auto lt_ent = LT::GetEntry(lt, pos);
		char before = LT::GetEntry(lt, lt.size - 2).lexema;
		IT::Entry* ent = nullptr;
		if (before == LEX_COLON && lt_ent.lexema == LEX_ID) {
			ent = &IT::GetEntry(it, lt_ent.idxTi);
		}
		else if (before == LEX_ARROW && lt_ent.lexema == LEX_RIGHTHESIS) {
			do {
				if (--pos < 0) return;
				lt_ent = LT::GetEntry(lt, pos);
			} while (lt_ent.lexema != LEX_FUNCTION && lt_ent.lexema != LEX_MAIN);
			if (lt_ent.lexema == LEX_MAIN) {
				ent = &IT::GetEntry(it, lt_ent.idxTi);                
			}
			else {
				ent = &IT::GetEntry(it, LT::GetEntry(lt, pos + 1).idxTi);
			}
		}
		else {
			return;
		}

		if (ent->iddatatype != IT::UNDEF) {
			return;
		}
		if (words[ind].str == _NUMBER) {
			ent->iddatatype = IT::INT;
		}
		else if (words[ind].str == _TEXT) {
			ent->iddatatype = IT::STR;
		}
		else if (words[ind].str == _VOID) {
			ent->iddatatype = IT::VOID;
		}
	}

	lexer LexAnaliz(LT::LexTable& lextable, IT::IdTable& idtable, std::vector<In::WORD>& words) {
		inHeader = false;
		scopes.assign(1, "");
		loopCounter = 0;
		for (int i = 0; i < words.size(); i++)
		{
			if (Check(words[i].str, typeNumber))
			{
				LT::Entry ent = { LEX_NUMBER, words[i].line };
				LT::Add(lextable, ent);
				SetType(idtable, lextable, i, words);
			}
			else if (Check(words[i].str, typeText))
			{
				LT::Entry ent = { LEX_TEXT,  words[i].line };
				LT::Add(lextable, ent);
				SetType(idtable, lextable, i, words);
			}
			else if (Check(words[i].str, typeVoid))
			{
				LT::Entry ent = { LEX_VOID,  words[i].line };
				LT::Add(lextable, ent);
				SetType(idtable, lextable, i, words);
			}
			else if (Check(words[i].str, typeFunction))
			{
				LT::Entry ent = { LEX_FUNCTION,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeAs))
			{
				LT::Entry ent = { LEX_AS,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeAssign))
			{
				LT::Entry ent = { LEX_ASSIGN,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeArrow))
			{
				inHeader = false;
				LT::Entry ent = { LEX_ARROW,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeRepeat))
			{
				scopes.push_back(scopes.back() + "#" + std::to_string(loopCounter++));
				LT::Entry ent = { LEX_REPEAT,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeCapture))
			{
				LT::Entry ent = { LEX_CAPTURE,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeEnd))
			{
				LT::Entry ent = { LEX_END,  words[i].line };
				if (scopes.size() >= 1) {
					scopes.pop_back();
				}
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeProject))
			{
				LT::Entry ent = { LEX_PROJECT,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeReturn))
			{
				LT::Entry ent = { LEX_RETURN,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeMain))
			{
				unsigned int idx = IT::IsId(idtable, words[i].str, "");
				if (idx == TI_NULLIDX) {
					IT::Entry it_ent = { lextable.size, words[i].str, IT::IDDATATYPE::UNDEF, IT::IDTYPE::F, "" };
					IT::Add(idtable, it_ent);
					idx = idtable.size - 1;
				}
				LT::Entry ent = { LEX_MAIN, words[i].line, idx };
				LT::Add(lextable, ent);
				scopes.push_back("main");
				inHeader = true;
			}
			else if (Check(words[i].str, typeStringLiteral))
			{
				if (words[i].str.size() - 2 > 255) {
					ERROR_THROW_IN(120, words[i].line, words[i].col)
				}
				unsigned int idx = IT::IsId(idtable, words[i].str, "");
				if (idx == TI_NULLIDX) {
					IT::Entry it_ent = { lextable.size, words[i].str, IT::IDDATATYPE::STR, IT::IDTYPE::L, "" };
					std::string text = words[i].str.substr(1, words[i].str.size() - 2);      //            
					it_ent.value.vstr.len = (unsigned char)text.size();
					text.copy(it_ent.value.vstr.str, text.size());
					it_ent.value.vstr.str[text.size()] = '\0';
					idx = idtable.size;
					IT::Add(idtable, it_ent);
				}
				LT::Entry ent = { LEX_LITERAL,  words[i].line, idx };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeDecNumber) ||
				Check(words[i].str, typeBinNumber))
			{
				bool overflow;
				long long v = ParseInt(words[i].str, overflow);
				std::string key = overflow ? words[i].str : std::to_string(v);
				unsigned int idx = IT::IsId(idtable, key, "");
				if (idx == TI_NULLIDX) {
					IT::Entry it_ent = { lextable.size, key, IT::IDDATATYPE::INT, IT::IDTYPE::L, "" };
					it_ent.value.vint.val = (int)v;
					it_ent.value.vint.overflow = overflow;
					IT::Add(idtable, it_ent);
					idx = idtable.size - 1;
				}
				LT::Entry ent = { LEX_LITERAL, words[i].line, idx };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeRightThesis))
			{
				LT::Entry ent = { LEX_RIGHTHESIS,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeLeftThesis))
			{
				LT::Entry ent = { LEX_LEFTHESIS,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeSemicolon))
			{
				LT::Entry ent = { LEX_SEMICOLON,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeColon))
			{
				LT::Entry ent = { LEX_COLON,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeComma))
			{
				LT::Entry ent = { LEX_COMMA,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typePlus))
			{
				LT::Entry ent = { LEX_PLUS,  words[i].line,LT_TI_NULLIDX,'+' };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeMinus))
			{
				LT::Entry ent = { LEX_MINUS,  words[i].line,LT_TI_NULLIDX,'-' };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeMulti))
			{
				LT::Entry ent = { LEX_STAR,  words[i].line,LT_TI_NULLIDX,'*' };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeDiv))
			{
				LT::Entry ent = { LEX_DIRSLASH,  words[i].line,LT_TI_NULLIDX,'/' };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeEquals))
			{
				LT::Entry ent = { LEX_EQUALS,  words[i].line };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeMod))
			{
				LT::Entry ent = { LEX_SHARP,  words[i].line,LT_TI_NULLIDX,'#' };
				LT::Add(lextable, ent);
			}
			else if (Check(words[i].str, typeIdentificator))
			{
				if (words[i].str.size() > 32) {
					ERROR_THROW_IN(121, words[i].line, words[i].col);
				}
				unsigned int idx = AddIDtoTable(idtable, lextable, i, words);
				LT::Entry ent = { LEX_ID, words[i].line, idx };
				LT::Add(lextable, ent);
			}
			else {
				char c = words[i].str[0];
				if (c == '\'' || c == '\"') {
					ERROR_THROW_IN(122, words[i].line, words[i].col);
				}
				if (c >= '0' && c <= '9') {
					ERROR_THROW_IN(123, words[i].line, words[i].col);
				}
				ERROR_THROW_IN(119, words[i].line, words[i].col);
			}
			lextable.table[lextable.size - 1].col = words[i].col;                              
		}
		std::cout << std::setfill(' ');
		return { lextable,idtable };
	}
}
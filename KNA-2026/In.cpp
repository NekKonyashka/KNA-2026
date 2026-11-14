#include "stdafx.h"
#include "In.h"
#include <fstream>
#include "Error.h"
#include <vector>

namespace In {
	IN getin(wchar_t infile[], std::vector<std::pair<std::string, int>>& words) {
		std::ifstream file(infile, std::ios::binary);
		words.clear();
		if (!file.is_open()) {
			ERROR_THROW(110);
		}

		IN in{};

		in.size = file.seekg(0, file.end).tellg();

		char* buff = new char[in.size];
		std::string temp = "";
		unsigned char* result = new unsigned char[IN_MAX_LEN_TEXT];

		file.seekg(0L, file.beg);

		file.read(buff, in.size);

		char* t = new char[16];
		int InPosText = 0, curPos = 1;

		for (int i = 0; i < in.size; ++i)
		{
			unsigned char ch = buff[i];
			curPos++;
			switch (in.code[ch]) {
			case IN::F: {
				ERROR_THROW_IN(111, in.lines, curPos);
			}
			case IN::T: {
				temp += ch;
				result[InPosText++] = ch;
				break;
			}
			case IN::I: {
				continue;
			}
			case IN::O:
			case IN::S: {
				if (temp != "") {
					words.push_back({ temp, in.lines });
					temp = "";
				}

				if (ch == ' ' || ch == '\t') {
					if (InPosText > 0 && result[InPosText - 1] != ' ' && result[InPosText - 1] != '\n') {
						result[InPosText++] = ' ';
					}
					break;
				}

				if (ch == '.') {
					if (i + 2 < in.size && buff[i + 1] == '.' && buff[i + 2] == '.') {
						while (i + 1 < in.size && buff[i + 1] != '\n') {
							++i;
						}
						break;
					}
				}

				temp = ch;
				if (ch == '\'' || ch == '\"') {
					while(i + 1 < in.size && (buff[i + 1] != '\'' && buff[i + 1] != '\"')) {
						temp += buff[++i];
					}
					temp += buff[++i];
				}


				if ((ch == ':' && i + 1 < in.size && buff[i + 1] == '=') ||
					(ch == '-' && i + 1 < in.size && buff[i + 1] == '>')) {
					temp += buff[i + 1];
					++i;
					++curPos;
				}

				words.push_back({ temp, in.lines });
				if (InPosText != 0 && result[InPosText - 1] != '\n' && result[InPosText - 1] != ' ') {
					result[InPosText++] = ' ';
				}
				for (int k = 0; k < temp.size(); k++) {
					result[InPosText++] = temp[k];
				}
				if (i + 1 < in.size && buff[i + 1] != '\n') {
					result[InPosText++] = ' ';
				}
				temp = "";
				break;
			}
			default: {
				if (temp != "") {
					words.push_back({ temp, in.lines });
					temp = "";
				}
				if (result[InPosText - 1] != ' ') {
					result[InPosText++] = ' ';
				}
				result[InPosText++] = '|';
				_itoa(in.lines, t, 10);
				for (int l = 0; l < strlen(t); l++) {
					result[InPosText++] = t[l];
				}
				in.lines++;
				result[InPosText++] = ch;
				curPos = 1;
			}
			}
		}

		if (temp != "") {
			words.push_back({ temp, in.lines });
			temp = "";
		}

		if (result[InPosText - 1] != ' ') {
			result[InPosText++] = ' ';
		}
		result[InPosText++] = '|';
		_itoa(in.lines, t, 10);
		for (int l = 0; l < strlen(t); l++) {
			result[InPosText++] = t[l];
		}
		result[InPosText] = '\0';
		in.text = result;

		file.close();
		delete[] buff;
		delete[] t;
		return in;
	}

}
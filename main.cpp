#include <iostream>
#include <ctime>
#include <vector>

enum {
	lowercase = 0b0001,
	uppercase = 0b0010,
	numbers = 0b0100,
	specialCharacters = 0b1000
};

void Pwgen(std::string sym, int count) {
	std::string pw = "";
	for (int i = 0; i < count; i++) {
		pw += sym[rand() % sym.size()];
	}
	std::cout << pw << "\n";
}

std::string AddSym(unsigned char characterSet) {
	std::string sym = "";
	for (char i = 33; i <= 126; i++) {
		if (i >= 97 && i <= 122 && characterSet & lowercase)
			sym += i;
		else if (i >= 65 && i <= 90 && (characterSet & uppercase) >> 1)
			sym += i;
		else if (i >= 48 && i <= 57 && (characterSet & numbers) >> 2)
			sym += i;
		else if (((i >= 33 && i <= 47) || (i >= 58 && i <= 64) || (i >= 91 && i <= 96) || (i >= 123 && i <= 126)) && (characterSet & specialCharacters) >> 3)
			sym += i;
	}
	return sym;
}

void Help(std::string name) {
	std::cout << "Генератор паролей\n"
		<< "параметры запуска " << name << " [-l] [-u] [-n] [-s] [-c 10] [-p 10] [-h]\n"
		<< "-l строчные буквы\n"
		<< "-u заглавные буквы\n"
		<< "-n цифры\n"
		<< "-s спецсимволы\n"
		<< "-с [num] колличество знаков\n"
		<< "-p [num] колличество паролей\n"
		<< "-h справка\n";
}

int StrToInt(std::string str) {
	int tmp = 0;
	for (unsigned int i = 0; i < str.length(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			tmp *= 10;
			tmp += str[i] - '0';
		}
	}
	return tmp;
}

int main(int countArgs, char** arg) {
	setlocale(LC_ALL, "rus");
	std::srand((unsigned int)std::time(nullptr));
	unsigned char characterSet = 0;
	int numberSigns = 8;
	int numberPassword = 10;
	if (countArgs > 1) characterSet = 0;
	for (int i = 1; i < countArgs; i++) {
		std::string str = arg[i];
		if (str == "-l") characterSet |= lowercase;
		else if (str == "-u") characterSet |= uppercase;
		else if (str == "-n") characterSet |= numbers;
		else if (str == "-s") characterSet |= specialCharacters;
		else if (str == "-c" && i < countArgs - 1) numberSigns = StrToInt(arg[i + 1]);
		else if (str == "-p" && i < countArgs - 1) numberPassword = StrToInt(arg[i + 1]);
		else if (str == "-h") {
			Help(arg[0]);
			return 0;
		}
	}
	std::string sym = AddSym(characterSet);
	if (sym != "") {
		for (int i = 0; i < numberPassword; i++) {
			Pwgen(sym, numberSigns);
		}
	}
	else {
		Help(arg[0]);
		return 0;
	}
}
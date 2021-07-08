#include <iostream>
#include <ctime>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <thread>
#include <mutex>

enum {
	lowercase = 0b0001,
	uppercase = 0b0010,
	numbers = 0b0100,
	specialCharacters = 0b1000
};

std::mutex lockOutput;

void Pwgen(std::string sym, int count) {
	std::string pw = "";
	std::random_device rnd;
	std::seed_seq seed{ rnd() };
	std::mt19937 generator(seed);
	std::uniform_int_distribution<int> distribution(1, sym.size());
	for (int i = 0; i < count; i++) {
		int rnd = distribution(generator) - 1;
		pw += sym[rnd];
	}
	lockOutput.lock();
	std::cout << pw << "\n";
	lockOutput.unlock();
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
	std::string tmp = arg[0];
	std::string filename = tmp + ".ini";
	std::ifstream fileConfig(filename);
	std::vector<std::string> saveArg;
	if (fileConfig.is_open() && countArgs == 1) {
		while (!fileConfig.eof())
		{
			std::string str = "";
			fileConfig >> str;
			saveArg.push_back(str);
		}
		fileConfig.close();
	}
	else {
		for (int i = 1; i < countArgs; i++)
			saveArg.push_back(arg[i]);
	}
	unsigned char characterSet = 0;
	int numberSigns = 0;
	int numberPassword = 0;
	for (unsigned int i = 0; i < saveArg.size(); i++) {
		std::string str = saveArg[i];
		if (str == "-l") characterSet |= lowercase;
		else if (str == "-u") characterSet |= uppercase;
		else if (str == "-n") characterSet |= numbers;
		else if (str == "-s") characterSet |= specialCharacters;
		else if (str == "-c" && i < saveArg.size() - 1) numberSigns = StrToInt(saveArg[i + 1]);
		else if (str == "-p" && i < saveArg.size() - 1) numberPassword = StrToInt(saveArg[i + 1]);
		else if (str == "-h") {
			Help(arg[0]);
			return 0;
		}
	}
	std::string sym = AddSym(characterSet);
	std::vector<std::thread> passwords;
	if (sym != "" && numberSigns != 0 && numberPassword != 0) {
		for (int i = 0; i < numberPassword; i++)
			passwords.push_back(std::thread(Pwgen,sym, numberSigns));
		if (countArgs > 1) {
			std::ofstream fileConfig(filename);
			if (fileConfig.is_open()) {
				std::string str = "";
				for (int i = 1; i < countArgs; i++) {
					str += arg[i];
					str += " ";
				}
				fileConfig << str;
				fileConfig.close();
			}
		}
		for (unsigned int i = 0; i < passwords.size(); i++)
			passwords[i].join();
	}
	else {
		Help(arg[0]);
		return 0;
	}
}
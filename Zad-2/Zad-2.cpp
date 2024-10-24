#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <cstring>

std::string preprocess(const std::string& text) {
    std::string result;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            result += std::toupper(ch);
        }
    }
    return result;
}

std::string caesarCipher(const std::string& text, int key, bool encrypt = true) {
    std::string result;
    for (char ch : text) {
        if (encrypt) {
            result += 'A' + (ch - 'A' + key) % 26;
        } else {
            result += 'A' + (ch - 'A' - key + 26) % 26;
        }
    }
    return result;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        exit(1);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie można zapisać do pliku: " << filename << std::endl;
        exit(1);
    }
    file << content;
    file.close();
}

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile, keyFile;
    bool encryptMode = true;
    int key = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-i") == 0) {
            inputFile = argv[++i];
        } else if (std::strcmp(argv[i], "-o") == 0) {
            outputFile = argv[++i];
        } else if (std::strcmp(argv[i], "-k") == 0) {
            keyFile = argv[++i];
        } else if (std::strcmp(argv[i], "-e") == 0) {
            encryptMode = true;
        } else if (std::strcmp(argv[i], "-d") == 0) {
            encryptMode = false;
        }
    }

    std::string keyContent = readFile(keyFile);
    key = std::stoi(keyContent);

    std::string inputText = readFile(inputFile);
    std::string preprocessedText = preprocess(inputText);
    std::string resultText = caesarCipher(preprocessedText, key, encryptMode);

    writeFile(outputFile, resultText);

    std::cout << "Proces zakończony sukcesem. Wynik zapisano do: " << outputFile << std::endl;

    return 0;
}

#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>

std::string preprocess(const std::string& text) {
    std::string result;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            result += std::toupper(ch);
        }
    }
    return result;
}

int modularInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    throw std::invalid_argument("Brak odwrotności modularnej dla a.");
}

std::string affineCipher(const std::string& text, int a, int b, bool encrypt = true) {
    std::string result;
    int a_inv = 0;
    if (!encrypt) {
        a_inv = modularInverse(a, 26); 
    }
    
    for (char ch : text) {
        int x = ch - 'A';
        if (encrypt) {
            result += 'A' + (a * x + b) % 26;
        } else {
            result += 'A' + (a_inv * (x - b + 26)) % 26;
        }
    }
    return result;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
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
    int a = 0, b = 0;

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
    std::istringstream keyStream(keyContent);
    keyStream >> a >> b;

    if (gcd(a, 26) != 1) {
        std::cerr << "Wartość a musi być względnie pierwsza do 26." << std::endl;
        exit(1);
    }

    std::string inputText = readFile(inputFile);
    std::string preprocessedText = preprocess(inputText);
    std::string resultText = affineCipher(preprocessedText, a, b, encryptMode);

    // Zapis wyniku do pliku wyjściowego
    writeFile(outputFile, resultText);

    std::cout << "Proces zakończony sukcesem. Wynik zapisano do: " << outputFile << std::endl;

    return 0;
}

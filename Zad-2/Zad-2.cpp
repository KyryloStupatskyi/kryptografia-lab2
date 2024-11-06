#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstring>

#include <map>
#include <iomanip>
#include <limits>

#include <gsl/gsl_cdf.h>

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

// Funkcja do obliczania n-gramów
std::map <std::string, int> get_multigrams(const int order, const std::string& text) {
    std::map<std::string, int> multigram_map;

    for (int i = 0; i < text.length() - (order - 1); i++) {
        auto bg = multigram_map.find(text.substr(i, order));
        if (bg != multigram_map.end()) {
            bg->second++;
        }
        else {
            multigram_map.insert(std::pair<std::string, int>(text.substr(i, order), 1));
        }
    }
    return multigram_map;
}

// Funkcja do odczytu referencyjnej bazy n-gramów
std::map<std::string, double> read_reference_ngrams(const std::string& filename) {
    std::map<std::string, double> refNgramMap;
    std::map<std::string, double> countNgramMap;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening reference file: " << filename << std::endl;
        exit(1);
    }

    std::string ngram;
    int count = 0;
    double sum = 0;

    for (std::string line; getline(file, line);) {
        count  = stoi(line.substr(line.find(' ') + 1, line.length()));
        countNgramMap[line.substr(0, line.find(' '))] = count;
        sum += count;
    }
    for (const auto& p : countNgramMap) {
        refNgramMap.insert(std::pair<std::string, double>(p.first, p.second/sum));
    }

    file.close();
    return refNgramMap;
}

double calculate_chi_square(const std::map<std::string, int>& observed, const std::map<std::string, double>& expected, int totalNgrams) {
    double chiSquare = 0.0;
    for (const auto& pair : observed) {
        const std::string& ngram = pair.first;
        int observedCount = pair.second;

        if (expected.find(ngram) != expected.end()) {
            double expectedCount = expected.at(ngram) * totalNgrams;
            chiSquare += pow(observedCount - expectedCount, 2) / expectedCount;
        }
    }
    return chiSquare;
}

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile, keyFile;
    bool encryptMode = true, bruteForce = false;
    int key = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-i") == 0) {
            inputFile = argv[++i];
            std::cout << "Input file: " << inputFile << std::endl;
        } else if (std::strcmp(argv[i], "-o") == 0) {
            outputFile = argv[++i];
        } else if (std::strcmp(argv[i], "-k") == 0) {
            keyFile = argv[++i];
        } else if (std::strcmp(argv[i], "-e") == 0) {
            encryptMode = true;
        } else if (std::strcmp(argv[i], "-d") == 0) {
            encryptMode = false;
        } else if (std::strcmp(argv[i], "-a") == 0) {
            if(std::strcmp(argv[++i], "bf") == 0)
                bruteForce = true;
        }
    }

    std::string inputText = readFile(inputFile);
    std::string preprocessedText = preprocess(inputText);

    if (bruteForce) {
        int best_shift = -1;
        double threshold = gsl_cdf_chisq_Pinv(0.95, 25);
        double best_chi_squared = std::numeric_limits<double>::max();

        //int ngramOrder = refNgramMode - '0';
        std::string candidateText;
        for (int i = 0; i < 26; ++i) {
            candidateText = caesarCipher(preprocessedText, i, encryptMode);
            for (int ngramOrder = 1; ngramOrder <= 4; ++ngramOrder) {
                std::string referenceFile;
                switch (ngramOrder) {
                    case 1:
                        referenceFile = "eng-n-grams/english_monograms.txt";
                    break;
                    case 2:
                        referenceFile = "eng-n-grams/english_bigrams.txt";
                    break;
                    case 3:
                        referenceFile = "eng-n-grams/english_trigrams.txt";
                    break;
                    case 4:
                        referenceFile = "eng-n-grams/english_quadgrams.txt";
                    break;
                }

                std::map<std::string, int> observedNgrams = get_multigrams(ngramOrder, candidateText);
                std::map<std::string, double> referenceNgrams = read_reference_ngrams(referenceFile);

                int totalNgrams = candidateText.length() - (ngramOrder - 1);
                double chiSquare = calculate_chi_square(observedNgrams, referenceNgrams, totalNgrams);

                if (chiSquare < threshold && chiSquare < best_chi_squared) {
                    best_chi_squared = chiSquare;
                    best_shift = i;
                }
            }

        }

        if (best_shift != -1) {
            std::cout << "Best shift: " << best_shift << std::endl;
            std::cout << "Decrypted text: " << caesarCipher(preprocessedText, best_shift, encryptMode) << std::endl;
            writeFile(outputFile, caesarCipher(preprocessedText, best_shift, encryptMode));
            std::cout << "Chi-squared value: " << best_chi_squared << std::endl;
        } else {
            std::cout << "No suitable shift found within the threshold." << std::endl;
        }

    }
    else {
        std::string keyContent = readFile(keyFile);
        key = std::stoi(keyContent);

        std::string resultText = caesarCipher(preprocessedText, key, encryptMode);

        writeFile(outputFile, resultText);

        std::cout << "Proces zakończony sukcesem. Wynik zapisano do: " << outputFile << std::endl;
    }


    return 0;

}

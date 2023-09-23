#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <set>
#include <map>
#include <vector>
#include <chrono>
using namespace std;
clock_t tStart;

struct InsurancePolicy {
    int policyNumber; 
    char company;
    char ownerLastName;
};

int generateUniquePolicyNumber(set<int>& usedPolicyNumbers) {
    int policyNumber;
    do {
        policyNumber = rand() % 1000000; 
    } while (usedPolicyNumbers.count(policyNumber) > 0); 
    usedPolicyNumbers.insert(policyNumber); 
    return policyNumber;
}

int createFile() 
{
    ofstream textFile("C://Users//Елена//Desktop//ВУЗ//СИАОД//Практическая работа 2//insurance_policies.txt");
    if (!textFile.is_open())
    {
        cerr << "Ошибка открытия файла для чтения!" << endl;
        return 1;
    }
    set<int> usedPolicyNumbers;
    srand(time(0));

    int numberOfPolicies;
    cout << "Введите количество полисов: ";
    cin >> numberOfPolicies;
    for (int i = 0; i < numberOfPolicies; i++) {
        InsurancePolicy policy;
        policy.policyNumber = generateUniquePolicyNumber(usedPolicyNumbers);
        //cin >> policy.company;
        //cin >> policy.ownerLastName;
        policy.company = 'a';
        policy.ownerLastName = 'a';

        textFile << policy.policyNumber << " " << policy.company << " " << policy.ownerLastName << "\n";
    }

    

    textFile.close();

    return 0;
}

int fileToBin()
{
    ifstream textFile("C://Users//Елена//Desktop//ВУЗ//СИАОД//Практическая работа 2//insurance_policies.txt");
    ofstream binaryFile("C://Users//Елена//Desktop//ВУЗ//СИАОД//Практическая работа 2//insurance_policies.dat", ios::binary);

    if (!textFile.is_open() || !binaryFile.is_open()) {
        cerr << "Ошибка открытия файлов!" << endl;
        return 1;
    }

    int policyNumber;
    char company;
    char ownerLastName;

    while (textFile >> policyNumber >> company >> ownerLastName) {
        InsurancePolicy policy;
        policy.policyNumber = policyNumber;
        policy.company = company;
        policy.ownerLastName = ownerLastName;

        binaryFile.write(reinterpret_cast<char*>(&policy), sizeof(InsurancePolicy));
    }
    textFile.close();
    binaryFile.close();

    return 0;
};
    
void findPolicyFile()
{
    int policyNumberToFind;
    cout << "Введите номер полиса для поиска: ";
    cin >> policyNumberToFind;
    tStart = clock();
    ifstream binaryFile("C://Users//Елена//Desktop//ВУЗ//СИАОД//Практическая работа 2//insurance_policies.dat", ios::binary);

    if (!binaryFile.is_open()) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }

    InsurancePolicy policyToFind;
    bool isFound = false;

    while (binaryFile.read(reinterpret_cast<char*>(&policyToFind), sizeof(InsurancePolicy))) {

        if (policyToFind.policyNumber == policyNumberToFind) {
            cout << "Найден полис с номером " << policyToFind.policyNumber << ":" <<endl;
            cout << "Компания: " << policyToFind.company << endl;
            cout << "Фамилия владельца: " << policyToFind.ownerLastName << endl;

            isFound = true;
            break;
        }
    }

    binaryFile.close();

    if (!isFound) {
        cout << "Полис с номером " << policyNumberToFind << " не найден." << endl;
    }
}



InsurancePolicy readRecordFromFile(const string& filename, streampos position) {
    ifstream binaryFile(filename, ios::binary);

    if (!binaryFile.is_open()) {
        cerr << "Ошибка открытия файла!" << endl;
        return {};
    }

    binaryFile.seekg(position);
    InsurancePolicy policy;

    if (binaryFile.read(reinterpret_cast<char*>(&policy), sizeof(InsurancePolicy))) {
        binaryFile.close();
        return policy;
    }

    binaryFile.close();
    return {};
}


map<int, streampos> createIndexTable(const string& filename) {
    map<int, streampos> indexTable;
    ifstream binaryFile(filename, ios::binary);

    if (!binaryFile.is_open()) {
        cerr << "Ошибка открытия файла!" << endl;
        return indexTable;
    }

    int policyNumber;
    streampos position = 0;

    while (binaryFile.read(reinterpret_cast<char*>(&policyNumber), sizeof(int))) {
        indexTable[policyNumber] = position;
        position = binaryFile.tellg();
    }

    binaryFile.close();
    return indexTable;
}

streampos findRecordPosition(const map<int, streampos>& indexTable, int policyNumber) {
    auto it = indexTable.find(policyNumber);
    if (it != indexTable.end()) {
        return it->second;
    }
    return -1;
}




void findPolicyTable() {
    const string& file = "C://Users//Елена//Desktop//ВУЗ//СИАОД//Практическая работа 2//insurance_policies.dat";
    int policyNumberToFind;
    cout << "Введите номер полиса для поиска: ";
    cin >> policyNumberToFind;
    tStart = clock();
    map<int, streampos> indexTable = createIndexTable(file);
    streampos recordPosition = findRecordPosition(indexTable, policyNumberToFind);

    if (recordPosition != -1) {
        InsurancePolicy foundPolicy = readRecordFromFile(file, recordPosition);
        cout << "Найден полис с номером " << foundPolicy.policyNumber << ":" << endl;
        cout << "Компания: " << foundPolicy.company << endl;
        cout << "Фамилия владельца: " << foundPolicy.ownerLastName << endl;
    }
    else {
        cout << "Полис с номером " << policyNumberToFind << " не найден." << endl;
    }
}

int main()
{
    setlocale(LC_ALL, "RUS");
    if (createFile()) return 1;
    if (fileToBin()) return 1;
    //findPolicyFile();
    findPolicyTable();
    printf("Time taken: %.3fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
    return 0;
}

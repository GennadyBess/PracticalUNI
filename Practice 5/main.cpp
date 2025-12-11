#include <iostream>
#include <fstream>
#include <iomanip>   
#include <ctime>    
#include <sstream>   


class intArray {
private:
    int *pArr = nullptr;
    int size = 0;

public:
    intArray(int n) {
        size = n;
        pArr = new int[size];
        for (int i = 0; i < size; ++i) {
            pArr[i] = 0;
        }
    }
    
    ~intArray() {
        delete[] pArr;
    }

    int getSize() {
        return size;
    }

    void copy(intArray &arr) {
        if (size != arr.size) {
        std::cout << "Ошибка: Размеры массивов не совпадают" << std::endl;
        return;
        }

        int size2 = arr.getSize();
        for (int i = 0; i < size2; i++) {
            pArr[i] = arr.pArr[i];
        }
    }

    void print() {
        for (int i = 0; i < size; i++) {
            std::cout << pArr[i] << " ";
        }
        std::cout << std::endl;
    }

    void setValue(int index, int value) {
        if (index < 0 || index >= size) {
            std::cout << "Ошибка: Выход за пределы массива" << std::endl;
            return;
        }
        if (value < -100 || value > 100) {
            std::cout << "Ошибка: Значение должно лежать в отрезке от -100 до 100" << std::endl;
            return;
        }
        pArr[index] = value;
    }

    int getValue(int index) {
        if (index < 0 || index >= size) {
            std::cout << "Ошибка: Выход за пределы массива" << std::endl;
            return 0;
        }
        return pArr[index];
    }

    void addValue(int value) {
        if (value < -100 || value > 100) {
            std::cout << "Ошибка: Значение должно лежать в отрезке от -100 до 100" << std::endl;
            return;
        }
        int* newData = new int[size+1];
        for (int i = 0; i < size; i++) {
            newData[i] = pArr[i];
        }
        newData[size] = value;

        delete[] pArr;
        pArr = newData;
        size++;
    }

    void add(intArray &arr) {
        int min = std::min(size, arr.getSize());
        for (int i = 0; i < min; i++) {
            pArr[i] = pArr[i] + arr.pArr[i];
        }
    }

    void subtract(intArray &arr) {
        int min = std::min(size, arr.getSize());
        for (int i = 0; i < min; i++) {
            pArr[i] = pArr[i] - arr.pArr[i];
        }
    }

    virtual void saveToFile() {}
};

class ArrTxt : public intArray {
public:
    ArrTxt(int n) : intArray(n) {}

    void saveToFile() override {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);

        std::ostringstream filename;
        filename << std::put_time(now, "%Y-%m-%d_%H-%M-%S") << ".txt";

        std::ofstream out(filename.str());
        for (int i = 0; i < getSize(); i++) {
            out << getValue(i) << " ";
        }

        out.close();
        std::cout << "Массив сохранён в файл " << filename.str() << std::endl;
    }
};

class ArrCSV : public intArray {
public:
    ArrCSV(int n) : intArray(n) {}

    void saveToFile() override {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);

        std::ostringstream filename;
        filename << std::put_time(now, "%Y-%m-%d_%H-%M-%S") << ".csv";

        std::ofstream out(filename.str());

        for (int i = 0; i < getSize(); i++) {
            out << getValue(i);
            if (i < getSize() - 1) out << ","; 
        }

        out.close();
        std::cout << "Массив сохранён в CSV файл " << filename.str() << std::endl;
    }
};

int main() {
    ArrCSV arr(5);
    arr.setValue(0, 10);
    arr.setValue(1, 66);
    arr.setValue(2, 34);
    arr.setValue(3, 44);
    arr.setValue(4, 10);

    arr.saveToFile(); 

    ArrTxt arr2(5);
    arr2.setValue(0, 4);
    arr2.setValue(1, 5);
    arr2.setValue(2, 70);
    arr2.setValue(3, 46);
    arr2.setValue(4, 59);

    arr2.saveToFile();
}
#include <iostream>

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
            throw std::out_of_range("Ошибка: выход за пределы массива");
        }
        if (value < -100 || value > 100) {
            throw std::invalid_argument("Ошибка: значение должно быть от -100 до 100");
        }
        pArr[index] = value;
    }

    int getValue(int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Ошибка: выход за пределы массива");
        }
        return pArr[index];
    }

    void addValue(int value) {
        if (value < -100 || value > 100) {
            throw std::invalid_argument("Ошибка: значение должно быть от -100 до 100");
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

};

int main() {
    intArray arr(3);
    try {
        arr.setValue(0, 50);
        arr.setValue(3, 10);
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
    }

    try {
        arr.setValue(1, 200); 
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
    }
}
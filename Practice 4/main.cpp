#include <iostream>
#include <algorithm>

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
};

class specArray : public intArray {
public:
    specArray(int n) : intArray(n) {}

    void bubbleSort(intArray &arr) {
        int n = arr.getSize();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - 1 - i; j++) {
                if (arr.getValue(j) > arr.getValue(j + 1)) {
                    int a = arr.getValue(j);
                    int b = arr.getValue(j + 1);
                    arr.setValue(j, b);
                    arr.setValue(j + 1, a);
                }
            }
        }
    }

    double mean() {
        int size = getSize();
        if (size == 0) return 0.0;

        double sum = 0;
        for (int i = 0; i < size; i++) {
            sum += getValue(i);
        }
        return sum / size; 
    }

    double median() {
        int size = getSize();
        if (size == 0) return 0;
        
        specArray temp(size);
        temp.copy(*this);
        bubbleSort(temp);

        if (size % 2 == 1) {
            return temp.getValue(size/2);
        }
        return (temp.getValue(size/2 - 1) + temp.getValue(size/2)) / 2.0;
    }
    
    int max() {
        int maxValue = getValue(0); 
        for (int i = 1; i < getSize(); i++) {
            if (getValue(i) > maxValue) maxValue = getValue(i);
        }
        return maxValue;
    }

    int min() {
        int minValue = getValue(0); 
        for (int i = 1; i < getSize(); i++) {
            if (getValue(i) < minValue) minValue = getValue(i);
        }
        return minValue;
    }
};

int main() {
    specArray arr1(5);
    arr1.setValue(0, 11);
    arr1.setValue(1, 7);
    arr1.setValue(2, 0);
    arr1.setValue(4, 56);
    arr1.setValue(5, 2);

    std::cout << "Среднее: " << arr1.mean() << std::endl;
    std::cout << "Медиана: " << arr1.median() << std::endl;
    std::cout << "Максимальное значение: " << arr1.max() << std::endl;
    std::cout << "Минимальное значение: " << arr1.min() << std::endl;


}

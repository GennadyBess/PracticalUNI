#include <iostream>
#include <cmath>
#include <typeinfo>
#include <stdexcept>

template <typename T>

class Array {
private:
    T *pArr = nullptr;
    int size = 0;

public:
    Array(int n) {
        size = n;
        pArr = new T[size];
    }
    
    ~Array() {
        delete[] pArr;
    }

    int getSize() { 
        return size;
    }

    T& getValue(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("Индекс вне диапазона");
        return pArr[index];
    }

    void setValue(int index, T value) {
        if (index < 0 || index >= size)
            throw std::out_of_range("Индекс вне диапазона");

        if constexpr (std::is_arithmetic<T>::value) { 
            if (value < -100 || value > 100) 
                throw std::invalid_argument("Значение должно быть в диапазоне от -100 до 100");
        }

        pArr[index] = value;
    }

    friend std::ostream& operator<<(std::ostream& os, Array<T>& arr) {
        os << "[";
        for (int i = 0; i < arr.size; ++i) {
            os << arr.pArr[i];
            if (i != arr.size - 1) os << ", ";
        }
        os << "]";
        return os;
    }

    double distance(Array<T>& other) {
        if (size != other.getSize())
            throw std::invalid_argument("Размеры массивов должны совпадать");

        if constexpr (!std::is_arithmetic<T>::value) {
            throw std::bad_typeid();
        } else {
        double sum = 0;
        for (int i = 0; i < size; i++) {
            double diff = static_cast<double>(pArr[i]) - static_cast<double>(other.getValue(i));
            sum += diff * diff;
        }
        return std::sqrt(sum);
        }
    }
};

int main() {

    try {
        Array<int> arr1(3);
        Array<int> arr2(3);

        arr1.setValue(0, 10);
        arr1.setValue(1, 20);
        arr1.setValue(2, 30);

        arr2.setValue(0, 5);
        arr2.setValue(1, 25);
        arr2.setValue(2, 35);

        std::cout << "arr1: " << arr1 << std::endl;
        std::cout << "arr2: " << arr2 << std::endl;

        std::cout << "Distance: " << arr1.distance(arr2) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    try {
        Array<std::string> strArr(2);
        strArr.setValue(0, "Hello");
        strArr.setValue(1, "World");

        std::cout << "strArr: " << strArr << std::endl;

        std::cout << "Distance: " << std::endl;
        strArr.distance(strArr);

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

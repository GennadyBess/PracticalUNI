#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>
#include <filesystem>

struct Image {
    int width = 0;
    int height = 0;
    int maxVal = 255;
    std::vector<int> data;
};

Image loadPGM(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    Image img;
    std::string format;
    file >> format;
    file >> img.width >> img.height;
    file >> img.maxVal;

    img.data.resize(img.width * img.height);
    for (int& p : img.data)
        file >> p;

    return img;
}

void savePGM(const std::string& filename, const Image& img) {
    std::ofstream file(filename);
    file << "P2\n";
    file << img.width << " " << img.height << "\n";
    file << img.maxVal << "\n";

    for (int i = 0; i < img.data.size(); i++) {
        file << img.data[i] << " ";
        if ((i + 1) % img.width == 0)
            file << "\n";
    }
}


void addSaltPepperNoise(Image& img, double probability) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int& p : img.data) {
        double r = dis(gen);
        if (r < probability / 2)
            p = 0;
        else if (r < probability)
            p = img.maxVal;
    }
}


int meanFilterPixel(const Image& img, int x, int y, int k) {
    int sum = 0, count = 0;

    for (int dy = -k; dy <= k; dy++)
        for (int dx = -k; dx <= k; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < img.width && ny < img.height) {
                sum += img.data[ny * img.width + nx];
                count++;
            }
        }
    return sum / count;
}

int medianFilterPixel(const Image& img, int x, int y, int k) {
    std::vector<int> values;

    for (int dy = -k; dy <= k; dy++)
        for (int dx = -k; dx <= k; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < img.width && ny < img.height) {
                values.push_back(img.data[ny * img.width + nx]);
            }
        }

    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

int weightedFilterPixel(const Image& img, int x, int y) {
    static const int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    int sum = 0;
    int weightSum = 16;

    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && ny >= 0 && nx < img.width && ny < img.height) {
                sum += img.data[ny * img.width + nx] * kernel[dy + 1][dx + 1];
            }
        }
    return sum / weightSum;
}

Image applyFilter(const Image& img, const std::string& type, int k = 1) {
    Image out = img;

    for (int y = 0; y < img.height; y++)
        for (int x = 0; x < img.width; x++) {
            int value = 0;

            if (type == "mean")
                value = meanFilterPixel(img, x, y, k);
            else if (type == "median")
                value = medianFilterPixel(img, x, y, k);
            else if (type == "weighted")
                value = weightedFilterPixel(img, x, y);

            out.data[y * img.width + x] = value;
        }

    return out;
}


double mse(const Image& a, const Image& b) {
    double sum = 0.0;
    for (int i = 0; i < a.data.size(); i++) {
        double d = a.data[i] - b.data[i];
        sum += d * d;
    }
    return sum / a.data.size();
}

double psnr(const Image& original, const Image& filtered) {
    double m = mse(original, filtered);
    if (m == 0.0) return 1e9;
    return 10.0 * std::log10((original.maxVal * original.maxVal) / m);
}


int main() {
    std::filesystem::create_directories("images");
    std::filesystem::create_directory("output");
    std::cout << "Processing started..." << std::endl;
    std::ofstream csv("results.csv");
    csv << "image,filter,kernel,noise,psnr\n";

    std::vector<double> noiseLevels = {0.05, 0.1, 0.2};
    std::vector<std::pair<std::string, int>> filters = {
        {"mean", 1},
        {"median", 1},
        {"weighted", 1}
    };

    for (const auto& entry : std::filesystem::directory_iterator("images")) {
        if (entry.path().extension() != ".pgm")
            continue;

        std::string name = entry.path().stem().string();
        Image original = loadPGM(entry.path().string());

        for (double noise : noiseLevels) {
            Image noisy = original;
            addSaltPepperNoise(noisy, noise);

            std::string noisyName =
                "output/" + name + "_noise_" + std::to_string(noise) + ".pgm";
            savePGM(noisyName, noisy);

            for (auto& f : filters) {
                Image filtered = applyFilter(noisy, f.first, f.second);
                double value = psnr(original, filtered);

                std::string outName =
                    "output/" + name + "_" + f.first +
                    "_n" + std::to_string(noise) + ".pgm";

                savePGM(outName, filtered);

                csv << name << ","
                    << f.first << ","
                    << f.second << ","
                    << noise << ","
                    << value << "\n";
            }
        }
    }

    csv.close();
    std::cout << "Processing finished." << std::endl;
    return 0;
}

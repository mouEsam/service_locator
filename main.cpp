#include "dynamic_service_locator.h"
#include "static_service_locator.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>

class IMathService {
public:
  virtual int get_int() const = 0;
  virtual ~IMathService(){};
};

class MathService : public IMathService {
public:
  MathService() = default;
  int get_int() const override { return 0; }
};

template <class LocatorType>
void locator_test(LocatorType &locator, int iterations) {
  for (int i = 0; i < iterations; i++) {
    locator.registerInstance(std::make_shared<int>(30000));
    locator.registerInstance(std::make_shared<float>(30000));
    locator.registerInstance(std::make_shared<double>(30000));
    locator.registerInstance(std::make_shared<char>(30000));
    locator.registerInstance(std::make_shared<unsigned char>(30000));
    locator.registerInstance(std::make_shared<unsigned int>(30000));
    locator.registerInstance(std::make_shared<unsigned long>(30000));
    locator.template registerInstance<IMathService>(
        std::make_shared<MathService>());

    locator.template registerFactory<int>(
        []() { return std::make_shared<int>(30000); });
    locator.template registerFactory<float>(
        []() { return std::make_shared<float>(30000); });
    locator.template registerFactory<double>(
        []() { return std::make_shared<double>(30000); });
    locator.template registerFactory<char>(
        []() { return std::make_shared<char>(30000); });
    locator.template registerFactory<unsigned char>(
        []() { return std::make_shared<unsigned char>(30000); });
    locator.template registerFactory<unsigned int>(
        []() { return std::make_shared<unsigned int>(30000); });
    locator.template registerFactory<unsigned long>(
        []() { return std::make_shared<unsigned long>(30000); });
    locator.template registerFactory<IMathService>(
        std::make_shared<MathService>);

    *locator.template resolve<int>();
    *locator.template resolve<int>();
    *locator.template resolve<float>();
    *locator.template resolve<float>();
    *locator.template resolve<double>();
    *locator.template resolve<double>();
    *locator.template resolve<char>();
    *locator.template resolve<char>();
    *locator.template resolve<unsigned char>();
    *locator.template resolve<unsigned char>();
    *locator.template resolve<unsigned int>();
    *locator.template resolve<unsigned int>();
    *locator.template resolve<unsigned long>();
    *locator.template resolve<unsigned long>();
    *locator.template resolve<IMathService>();
    *locator.template resolve<IMathService>();
    *locator.template resolve<IMathService>();
    *locator.template resolve<IMathService>();
    *locator.template resolve<IMathService>();
  }
}

void static_main(int iterations) {
  StaticServiceLocator locator;
  locator_test(locator, iterations);
}

void dynamic_main(int iterations) {
  DynamicServiceLocator locator;
  locator_test(locator, iterations);
}

void measure(std::function<void()> operation) {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto t1 = high_resolution_clock::now();
  operation();
  auto t2 = high_resolution_clock::now();

  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  duration<double, std::milli> ms_double = t2 - t1;

  std::cout << ms_int.count() << "ms\n";
  std::cout << ms_double.count() << "ms\n";
}

int main() {
  int iterations = 10000;
  std::cout << "Static" << std::endl;
  measure(std::bind(static_main, iterations));
  std::cout << "Dynamic" << std::endl;
  measure(std::bind(dynamic_main, iterations));
}
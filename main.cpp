#include "double_dispatch_service_locator.h"
#include "single_dispatch_service_locator.h"
#include "dynamic_service_locator.h"
#include "static_service_locator.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>


const static int number = 30;

class IMathService {
public:
  virtual int get_int() const = 0;
  virtual ~IMathService(){};
};

class MathService : public IMathService {
public:
  MathService() = default;
  int get_int() const override { return number; }
};

template <class LocatorType>
int locator_test(LocatorType &locator, int iterations) {

  int result = 0;

  for (int i = 0; i < iterations; i++) {
    locator.registerInstance(std::make_shared<int>(number));
    locator.registerInstance(std::make_shared<float>(number));
    locator.registerInstance(std::make_shared<double>(number));
    locator.registerInstance(std::make_shared<char>(number));
    locator.registerInstance(std::make_shared<unsigned char>(number));
    locator.registerInstance(std::make_shared<unsigned int>(number));
    locator.registerInstance(std::make_shared<unsigned long>(number));
    locator.template registerInstance<IMathService>(
        std::make_shared<MathService>());

    locator.template registerFactory<int>(
        []() { return std::make_shared<int>(number); });
    locator.template registerFactory<float>(
        []() { return std::make_shared<float>(number); });
    locator.template registerFactory<double>(
        []() { return std::make_shared<double>(number); });
    locator.template registerFactory<char>(
        []() { return std::make_shared<char>(number); });
    locator.template registerFactory<unsigned char>(
        []() { return std::make_shared<unsigned char>(number); });
    locator.template registerFactory<unsigned int>(
        []() { return std::make_shared<unsigned int>(number); });
    locator.template registerFactory<unsigned long>(
        []() { return std::make_shared<unsigned long>(number); });
    locator.template registerFactory<IMathService>(
        std::make_shared<MathService>);

    result += number == *locator.template resolve<int>();
    result += number == *locator.template resolve<int>();
    result += number == *locator.template resolve<float>();
    result += number == *locator.template resolve<float>();
    result += number == *locator.template resolve<double>();
    result += number == *locator.template resolve<double>();
    result += number == *locator.template resolve<char>();
    result += number == *locator.template resolve<char>();
    result += number == *locator.template resolve<unsigned char>();
    result += number == *locator.template resolve<unsigned char>();
    result += number == *locator.template resolve<unsigned int>();
    result += number == *locator.template resolve<unsigned int>();
    result += number == *locator.template resolve<unsigned long>();
    result += number == *locator.template resolve<unsigned long>();
    result += locator.template resolve<IMathService>()->get_int() == number;
    result += locator.template resolve<IMathService>()->get_int() == number;
    result += locator.template resolve<IMathService>()->get_int() == number; 
    result += locator.template resolve<IMathService>()->get_int() == number; 
    result += locator.template resolve<IMathService>()->get_int() == number;
  }

  return result;
}

int static_main(int iterations) {
  StaticServiceLocator locator;
  return locator_test(locator, iterations);
}

int double_dispatch_main(int iterations) {
  DoubleDispatchServiceLocator locator;
  return locator_test(locator, iterations);
}

int single_dispatch_main(int iterations) {
  SingleDispatchServiceLocator locator;
  return locator_test(locator, iterations);
}

int dynamic_main(int iterations) {
  DynamicServiceLocator locator;
  return locator_test(locator, iterations);
}

int measure(std::function<int()> operation) {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto t1 = high_resolution_clock::now();
  const auto result = operation();
  auto t2 = high_resolution_clock::now();

  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  duration<double, std::milli> ms_double = t2 - t1;

  std::cout << ms_int.count() << "ms\n";
  std::cout << ms_double.count() << "ms\n";
  return result;
}

int main() {
  int iterations = 10000;
  std::cout << "Static" << std::endl;
  int result = measure(std::bind(static_main, iterations));
  std::cout << "result: " << result << std::endl;
  std::cout << "Double Dispatch" << std::endl;
  result = measure(std::bind(double_dispatch_main, iterations));
  std::cout << "result: " << result << std::endl;
  std::cout << "Single Dispatch" << std::endl;
  result = measure(std::bind(single_dispatch_main, iterations));
  std::cout << "result: " << result << std::endl;
  std::cout << "Dynamic" << std::endl;
  result = measure(std::bind(dynamic_main, iterations));
  std::cout << "result: " << result << std::endl;
}
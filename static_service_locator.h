#pragma once

#include <cstddef>
#include <cstdlib>
#include <deque>
#include <functional>
#include <memory>
#include <random>
#include <variant>

template <int SEED> class StaticServiceLocator;

template <class SERVICE_TYPE> class ServiceResolver {
  std::function<std::shared_ptr<SERVICE_TYPE>()> resolver;

public:
  template <int SEED>
  ServiceResolver(const StaticServiceLocator<SEED> &locator) {
    resolver = [&]() { return locator.template resolve<SERVICE_TYPE>(); };
  }
  std::shared_ptr<SERVICE_TYPE> resolve() const { return resolver(); }
};

template <int _SEED, class _SERVICE> class _HOLDER {
  friend class StaticServiceLocator<_SEED>;
  static std::shared_ptr<std::shared_ptr<_SERVICE>> service;
  _HOLDER() = delete;
};

template <int SEED, class SERVICE>
std::shared_ptr<std::shared_ptr<SERVICE>> _HOLDER<SEED, SERVICE>::service{};

template <int _SEED, class _SERVICE> class _FACTORY_HOLDER {
  friend class StaticServiceLocator<_SEED>;
  static std::shared_ptr<std::function<std::shared_ptr<_SERVICE>()>> factory;
  _FACTORY_HOLDER() = delete;
};

template <class _SERVICE> std::shared_ptr<_SERVICE> _defaultfactory() {
  return std::shared_ptr<_SERVICE>();
}

template <int SEED, class SERVICE>
std::shared_ptr<std::function<std::shared_ptr<SERVICE>()>>
    _FACTORY_HOLDER<SEED, SERVICE>::factory{};

template <int SEED = 0> class StaticServiceLocator {
  static const int seed = SEED;
  static std::weak_ptr<int> _shared_ref;
  static std::vector<std::weak_ptr<void>> _destrs;

  std::shared_ptr<int> _ref;

public:
  StaticServiceLocator() {
    if (auto strong = _shared_ref.lock()) {
      _ref = strong;
    } else {
      _ref = std::shared_ptr<int>(new int, [](int *ref) {
        while (!_destrs.empty()) {
          if (auto des = _destrs.back().lock()) {
            des.reset();
          }
          _destrs.pop_back();
        }
        delete ref;
      });
      _shared_ref = _ref;
    }
  }

  StaticServiceLocator(StaticServiceLocator &&other) {
    _ref = other._ref;
    other._ref.reset();
  }

  StaticServiceLocator(const StaticServiceLocator &other) { _ref = other._ref; }

  StaticServiceLocator &operator=(StaticServiceLocator &&other) {
    _ref = other._ref;
    other._ref.reset();
    return *this;
  }

  StaticServiceLocator &operator=(const StaticServiceLocator &other) {
    _ref = other._ref;
    return *this;
  }

  template <class SERVICE_TYPE> std::shared_ptr<SERVICE_TYPE> resolve() const {
    auto service = _HOLDER<SEED, SERVICE_TYPE>::service;
    if (service)
      return *service;
    auto factory = _FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory;
    if (factory)
      return (*factory)();
    return nullptr;
  }

  template <class SERVICE_TYPE>
  void registerInstance(std::shared_ptr<SERVICE_TYPE> service) {
    _HOLDER<SEED, SERVICE_TYPE>::service =
        std::make_shared<std::shared_ptr<SERVICE_TYPE>>(service);
    _destrs.push_back(_HOLDER<SEED, SERVICE_TYPE>::service);
  }

  template <class SERVICE_TYPE>
  void registerFactory(std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    _FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory =
        std::make_shared<std::function<std::shared_ptr<SERVICE_TYPE>()>>(
            factory);
    _destrs.push_back(_FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory);
  }

  template <class SERVICE_TYPE>
  void registerSingletonFactory(
      std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    _FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory =
        std::make_shared<std::function<std::shared_ptr<SERVICE_TYPE>()>>([=]() {
          auto service = factory();
          _HOLDER<SEED, SERVICE_TYPE>::service = service;
          return service;
        });
    _destrs.push_back(_HOLDER<SEED, SERVICE_TYPE>::service);
    _destrs.push_back(_FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory);
  }

  template <class SERVICE_TYPE> void clear() {
    _HOLDER<SEED, SERVICE_TYPE>::service.reset();
    _FACTORY_HOLDER<SEED, SERVICE_TYPE>::factory.reset();
  }
};

template <int SEED>
std::weak_ptr<int> StaticServiceLocator<SEED>::_shared_ref{};

template <int SEED>
std::vector<std::weak_ptr<void>> StaticServiceLocator<SEED>::_destrs{};

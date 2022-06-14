#pragma once

#include <cstddef>
#include <cstdlib>
#include <deque>
#include <functional>
#include <memory>
#include <random>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <variant>

class SingleDispatchServiceLocator;
template <class SERVICE> class _SHOLDER;
template <class SERVICE> class _FACTORY_SHOLDER;

class SServiceHolder {
public:
  virtual inline std::shared_ptr<void> resolve() const = 0;
  virtual ~SServiceHolder() {}
};

template <class SERVICE> class _SHOLDER : public SServiceHolder {
  friend class _HolderVisitor;
  friend class SingleDispatchServiceLocator;
  std::shared_ptr<SERVICE> service;

public:
  _SHOLDER(std::shared_ptr<SERVICE> service) : service(service) {}
  inline std::shared_ptr<void> resolve() const override {
      return service;
  }
};

template <class SERVICE> class _FACTORY_SHOLDER : public SServiceHolder {
  friend class _HolderVisitor;
  friend class SingleDispatchServiceLocator;
  std::function<std::shared_ptr<SERVICE>()> factory;

public:
  _FACTORY_SHOLDER(std::function<std::shared_ptr<SERVICE>()> factory)
      : factory(factory) {}
  inline std::shared_ptr<void> resolve() const override {
      return factory();
  }
};

class SingleDispatchServiceLocator {
  std::unordered_map<size_t, std::unique_ptr<SServiceHolder>> holders;

  template <class Class> static inline size_t hash_of() {
    return typeid(Class).hash_code();
  }

public:
  SingleDispatchServiceLocator() : holders{} {}

  template <class SERVICE_TYPE> std::shared_ptr<SERVICE_TYPE> resolve() const {
    const size_t hash = hash_of<SERVICE_TYPE>();
    auto itr1 = holders.find(hash);
    if (itr1 != holders.end()) {
      return std::static_pointer_cast<SERVICE_TYPE>(itr1->second->resolve());
    }
    return nullptr;
  }

  template <class SERVICE_TYPE>
  void registerInstance(std::shared_ptr<SERVICE_TYPE> service) {
    holders[hash_of<SERVICE_TYPE>()] =
        std::make_unique<_SHOLDER<SERVICE_TYPE>>(service);
  }

  template <class SERVICE_TYPE>
  void registerFactory(std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    holders[hash_of<SERVICE_TYPE>()] =
        std::make_unique<_FACTORY_SHOLDER<SERVICE_TYPE>>(factory);
  }

  template <class SERVICE_TYPE>
  void registerSingletonFactory(
      std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    auto newFactory = [factory = std::move(factory), this]() {
      auto service = factory();
      this->registerInstance(service);
      return service;
    };
    registerFactory<SERVICE_TYPE>(newFactory);
  }

  template <class SERVICE_TYPE> void clear() {
    holders.erase(hash_of<SERVICE_TYPE>());
  }

  void clearAll() { holders.clear(); }
};

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

class DoubleDispatchServiceLocator;
template <class SERVICE> class _DHOLDER;
template <class SERVICE> class _FACTORY_DHOLDER;

class ValueCapturer {
public:
  virtual void capture(std::shared_ptr<void> value) = 0;
  virtual ~ValueCapturer() {}
};

template <class VALUE> class _ValueCapturer : public ValueCapturer {
  std::shared_ptr<VALUE> value;

public:
  std::shared_ptr<VALUE> get_value() const { return value; }
  void capture(std::shared_ptr<void> value) override {
    this->value = std::static_pointer_cast<VALUE>(value);
  }
};

class _HolderVisitor {
public:
  _HolderVisitor() {}
  template <class SERVICE>
  inline void visit(const _DHOLDER<SERVICE> &holder,
                    ValueCapturer &capturer) const {
    return capturer.capture(holder.service);
  }
  template <class SERVICE>
  inline void visit(const _FACTORY_DHOLDER<SERVICE> &holder,
                    ValueCapturer &capturer) const {
    return capturer.capture(holder.factory());
  }
};

class ServiceHolder {
public:
  virtual inline void resolve(const _HolderVisitor &visitor,
                              ValueCapturer &capturer) const = 0;
  virtual ~ServiceHolder() {}
};

template <class SERVICE> class _DHOLDER : public ServiceHolder {
  friend class _HolderVisitor;
  friend class DoubleDispatchServiceLocator;
  std::shared_ptr<SERVICE> service;

public:
  _DHOLDER(std::shared_ptr<SERVICE> service) : service(service) {}
  inline void resolve(const _HolderVisitor &visitor,
                      ValueCapturer &capturer) const override {
    visitor.visit(*this, capturer);
  }
};

template <class SERVICE> class _FACTORY_DHOLDER : public ServiceHolder {
  friend class _HolderVisitor;
  friend class DoubleDispatchServiceLocator;
  std::function<std::shared_ptr<SERVICE>()> factory;

public:
  _FACTORY_DHOLDER(std::function<std::shared_ptr<SERVICE>()> factory)
      : factory(factory) {}
  inline void resolve(const _HolderVisitor &visitor,
                      ValueCapturer &capturer) const override {
    visitor.visit(*this, capturer);
  }
};

class DoubleDispatchServiceLocator {
  std::unordered_map<size_t, std::unique_ptr<ServiceHolder>> holders;
  _HolderVisitor visitor;

  template <class Class> static inline size_t hash_of() {
    return typeid(Class).hash_code();
  }

public:
  DoubleDispatchServiceLocator() : holders{}, visitor{} {}

  template <class SERVICE_TYPE> std::shared_ptr<SERVICE_TYPE> resolve() const {
    const size_t hash = hash_of<SERVICE_TYPE>();
    auto itr1 = holders.find(hash);
    if (itr1 != holders.end()) {
      _ValueCapturer<SERVICE_TYPE> capturer;
      itr1->second->resolve(visitor, capturer);
      return capturer.get_value();
    }
    return nullptr;
  }

  template <class SERVICE_TYPE>
  void registerInstance(std::shared_ptr<SERVICE_TYPE> service) {
    holders[hash_of<SERVICE_TYPE>()] =
        std::make_unique<_DHOLDER<SERVICE_TYPE>>(service);
  }

  template <class SERVICE_TYPE>
  void registerFactory(std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    holders[hash_of<SERVICE_TYPE>()] =
        std::make_unique<_FACTORY_DHOLDER<SERVICE_TYPE>>(factory);
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

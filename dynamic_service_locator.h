#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

class DynamicServiceLocator {
  std::unordered_map<size_t, std::shared_ptr<void>> instances;
  std::unordered_map<size_t, std::function<std::shared_ptr<void>()>> factories;

  template <class Class> static inline size_t hash_of() {
    return typeid(Class).hash_code();
  }

public:
  DynamicServiceLocator() : instances(), factories(){};
  ~DynamicServiceLocator() { clearAll(); }

  template <class SERVICE_TYPE> void clear() {
    instances.erase(hash_of<SERVICE_TYPE>());
    factories.erase(hash_of<SERVICE_TYPE>());
  }

  void clearAll() {
    instances.clear();
    factories.clear();
  }

  template <class SERVICE_TYPE>
  void registerInstance(std::shared_ptr<SERVICE_TYPE> instance) {
    const size_t hash = hash_of<SERVICE_TYPE>();
    instances.emplace(hash, instance);
  }

  template <class SERVICE_TYPE>
  void registerFactory(std::function<std::shared_ptr<SERVICE_TYPE>()> factory) {
    const size_t hash = hash_of<SERVICE_TYPE>();
    factories.emplace(hash, factory);
  }

  template <class SERVICE_TYPE> std::shared_ptr<SERVICE_TYPE> resolve() const {
    const size_t hash = hash_of<SERVICE_TYPE>();
    auto itr1 = instances.find(hash);
    if (itr1 != instances.end())
      return std::static_pointer_cast<SERVICE_TYPE>(itr1->second);

    auto itr2 = factories.find(hash);
    if (itr2 != factories.end())
      return std::static_pointer_cast<SERVICE_TYPE>(itr2->second());

    return nullptr;
  }
};
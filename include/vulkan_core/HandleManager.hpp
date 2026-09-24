#pragma once
#include <cstdint>
#include <memory>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

using DeletionFn = std::function<void()>;

class HandleManager{
public:
    void initialize(uint32_t width, uint32_t height, const std::string& title);
    void initialize_handlers();
    void fulfill_dependencies_requests();

    void defer(DeletionFn fn);
    void flush_deferred();

    template <typename T>
    T& find_handler_by_name(const std::string& name){
        auto it = std::find_if(
            m_handlers.begin(), m_handlers.end(),
            [&](const auto& pair){
                return pair.first == name;
            }
        );
        if(it == m_handlers.end())
            throw std::runtime_error("ERROR: Handler not found: " + name);

        T* handler = dynamic_cast<T*>(it->second.get());
        if(!handler)
            throw std::runtime_error("ERROR: Handler '" + name + "' is not one of the requested type");

        return *handler;
    }
private:
    template <typename T, typename = typename std::enable_if<std::is_base_of<VulkanHandler, T>::value>::type,
    typename... Args>
    void add_handler(Args&&... args){
        auto handler = std::make_unique<T>(std::forward<Args>(args)...);
        std::string name = handler->get_name();
        for(const auto& [extisting_name, ptr] : m_handlers){
            if(extisting_name == name){
                std::cerr << "WARNING: Duplicate handlers: " << ptr->get_name() << '\n';
                return;
            }
        }
        T* ptr = handler.get();
        m_handlers.emplace_back(name, std::move(handler));
        defer(ptr->get_deletion_fn());
    }
private:
    std::vector<std::pair<std::string, std::unique_ptr<VulkanHandler>>> m_handlers;
    std::vector<DeletionFn> m_deletion_queue{};
};

}

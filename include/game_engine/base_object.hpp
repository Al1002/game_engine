/**
 * @file obj_manager.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-14
 * @copyright Copyright (c) 2025
 */
#pragma once
#include "std_includes.hpp"
#include "vects.hpp" // Mathematical vectors

// defined here
class Object;

// extern
class Engine;
class HandlerI;

/**
 * @brief Base class for all game objects. 
 */
class Object : public std::enable_shared_from_this<Object>
{
    friend Engine;

protected:
    weak_ptr<Object> parent_view;
    map<string, shared_ptr<Object>> children_map; ///< allows named access to children
    list<shared_ptr<Object>> children; ///< allows indexed access to children
    string name;
    function<void(Object *)> init_behavior;
    function<void(Object *, double)> loop_behavior;
    list<shared_ptr<HandlerI>> handlers;
public:
    string desiredName;
    weak_ptr<Engine> engine_view;

    Object(string desiredName = "Object");

    virtual void init();

    virtual void loop(double delta);

    shared_ptr<Engine> getEngine();

    /**
     * @brief Get the object's parent. If orphan, returns an empty shared_ptr.
     * @return shared_ptr<Object> the object's parent
     */
    shared_ptr<Object> getParent();

    /**
     * @brief The name the node is given by the parrent. May be appended by an index if another child already has that name.
     * @return string 
     */
    string getDesiredName();

    string getName();

    /**
     * @brief Attach a handler to the object, and register it in the engine if it exists.
     * @param handle 
     */
    void attachHandler(shared_ptr<HandlerI> handle);
    

    void dettachHandler(shared_ptr<HandlerI> handle);
    
    /**
     * @brief Returns a deep copy of the object, its children, etc. The clone is registered in the systems the original is registered in.
     * @return shared_ptr<Object> 
     */
    /*virtual shared_ptr<Object> clone()
    {
        shared_ptr<Object> c = make_shared<Object>(this);
        
        for(auto child : children)
        {
            c->addChild(child->clone());
        }
        return c;
    }*/

    /**
     * @brief A callable which is called in the object's loop
     * @param behaviour
     */
    void attachInitBehaviour(function<void(Object *)> behavior);

    /**
     * @brief A callable which is called in the object's init
     * @param behaviour
     */
    void attachLoopBehaviour(function<void(Object *, double)> behavior);

    /**
     * @brief Add child to the object. Child is appended to the back of the child list.
     * @param child child object
     */
    void addChild(shared_ptr<Object> child);

    /**
     * @brief Short alias for addChild.
     * @param child child object
     */
    inline void add(shared_ptr<Object> child)
    {
        addChild(child);
    }

    /**
     * @brief Get child by index.
     * @param index position of the child in the child list
     * @return shared_ptr<Object>
     * @throws out_of_range exception if the child index is out of range
     */
    shared_ptr<Object> getChild(int index);

    /**
     * @brief Get child by index, downcast to the template type.
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template <typename T>
    inline shared_ptr<T> getChild(int index)
    {
        auto child = dynamic_pointer_cast<T>(getChild(index));
        if (child.get() == nullptr)
            throw std::runtime_error("Child not of specified class");
        return child;
    }

    /**
     * @brief Short alias for getChild.
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template <typename T = Object>
    inline shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }

    /**
     * @brief Get child by name.
     * @param path 
     * @return shared_ptr<Object>
     * @throws std::out_of_range 
     */
    shared_ptr<Object> getChild(string path);

    /**
     * @brief Get child by name.
     * @param path 
     * @return shared_ptr<Object>
     * @throws std::out_of_range 
     */
    template <typename T>
    inline shared_ptr<T> getChild(string path)
    {
        return dynamic_pointer_cast<T>(getChild(path));
    }

    /**
     * @brief Short alias of getChild
     * @tparam T
     * @param indices
     * @return shared_ptr<T>
     */
    template <typename T = Object>
    inline shared_ptr<T> get(string path)
    {
        return getChild<T>(path);
    }

    /**
     * @brief Remove child by index.
     * @param index position of the child in the child list
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if the child index is out of range
     */
    shared_ptr<Object> removeChild(int index);

    /**
     * @brief Remove child by name.
     * @param name name of the child to be removed
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if no child has that name
     */
    shared_ptr<Object> removeChild(string name);

};

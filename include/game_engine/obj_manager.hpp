
#include "std_includes.hpp"
#include "vects.hpp" // Mathematical vectors
#include "colors.h" // #defined RGB_COLORs

// defined here
class ObjectManager;

// extern
class Object;
class Event;
class EventDispatcher;
#include "objects.hpp"

class ObjectManager : public std::enable_shared_from_this<Engine>
{
    
    //thread_pool::static_pool workers;
    unordered_set<shared_ptr<Object>> bucket;
    unordered_set<shared_ptr<Object>> dead_bucket;
    shared_ptr<Object> root; ///< root object

public:
    /**
     * @brief Add object for updates and initialization
     * @param obj
     */
    void registerObj(shared_ptr<Object> obj);

    void unregisterObj(shared_ptr<Object> obj);

    void update(double delta);

    // Composition with root object
    
    inline void addChild(shared_ptr<Object> child)
    {
        root->addChild(child);
        registerObj(child);
    }

    /**
     * @brief Short alias for addChild.
     * @param child child object
     */
    inline void add(shared_ptr<Object> child)
    {
        addChild(child);
    }

    template <typename T = Object>
    inline shared_ptr<T> getChild(int index)
    {
        return root->getChild<T>(index);
    }

    /**
     * @brief Short alias for getChild().
     * @param index position of the child in the child list
     * @return shared_ptr<Object>
     * @throws out_of_range exception if the child index is out of range
     */
    template <typename T = Object>
    inline shared_ptr<T> get(int index)
    {
        return getChild<T>(index);
    }
#if 0
    template <typename T = Object>
    inline shared_ptr<T> getChild(std::vector<int> indices)
    {
        return root->getChild<T>(indices);
    }

    /**
     * @brief Short alias for getChild.
     * @param index position of the child in the child list
     * @tparam T the type to downcast the child to
     * @return shared_ptr<Object>
     * @throws std::out_of_range exception if the child index is out of range
     * @throws  exception if the type is not an ancestor of the child's actual type
     */
    template <typename T>
    inline shared_ptr<T> get(std::vector<int> indices)
    {
        return getChild<T>(indices);
    }
#endif
    template <typename T = Object2D>
    inline shared_ptr<T> getChild(string path)
    {
        return root->getChild<T>(path);
    }

    template <typename T>
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
    inline shared_ptr<Object> removeChild(int index)
    {
        return root->removeChild(index);
    }

    /**
     * @brief Remove child by name.
     * @param name name of the child to be removed
     * @return shared_ptr<Object> the removed child
     * @throws std::out_of_range exception if no child has that name
     */
    inline shared_ptr<Object> removeChild(string name)
    {
        return root->removeChild(name);
    }
};

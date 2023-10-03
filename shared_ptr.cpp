#include <iostream>
#include <memory>
#include <functional>  // Include this line

namespace aremn {

template <typename T>
class shared_ptr {
private:
    struct ControlBlock {
        T* ptr;
        std::function<void(T*)> deleter;
        ControlBlock(T* p, std::function<void(T*)> d) : ptr(p), deleter(d) {}
    };

    int* ptr_count;
    ControlBlock* control;

public:
    shared_ptr() : control(nullptr), ptr_count(nullptr) {}

    explicit shared_ptr(ControlBlock * c) : control(c), ptr_count(new int(1)) {
    }

    ~shared_ptr() {
        if(ptr_count) {
            (*ptr_count)--;
            if (*ptr_count == 0) {
                control->deleter(control->ptr);
                delete control;
                delete ptr_count;
            }
        }
    }

    shared_ptr(const shared_ptr<T>& other) : control(other.control), ptr_count(other.ptr_count) {
        if(ptr_count) {
            (*ptr_count)++;
        }
    }

    shared_ptr<T>& operator=(const shared_ptr<T>& other) {
        if (this == &other) {
            return *this;
        }
        if(ptr_count) {
            (*ptr_count)--;
            if (*ptr_count == 0) {
                control->deleter(control->ptr);
                delete control;
                delete ptr_count;
            }
        }

        control = other.control;
        ptr_count = other.ptr_count;
        if (ptr_count) {
            (*ptr_count)++;
        }

        return *this;
    }

    T& operator*() {
        return *(control->ptr);
    }

    T* operator->() {
        return control->ptr;
    }

    void reset() {
        if (ptr_count) {
            (*ptr_count)--;
            if(*ptr_count == 0) {
                control->deleter(control->ptr);
                delete control;
                delete ptr_count;
            }
        }

        control = nullptr;
        ptr_count = nullptr;
    }

    size_t use_count() const {
        if (ptr_count) {
            return *ptr_count;
        }
        return 0;
    }

    template <typename U, typename... Args>
    static shared_ptr<T> make_shared(Args&&... args) {
        auto deleter = [](T* ptr) { delete static_cast<U*>(ptr); };
        auto* cb = new ControlBlock(new U(std::forward<Args>(args)...), deleter);
        return shared_ptr<T>(cb);
    }
};

}

class Base {
public:
    Base() {
        std::cout << "Base constructor" << std::endl;
    }

    ~Base() {
        std::cout << "Base destructor" << std::endl;
    }
};

class Derived : public Base {
public:
    Derived() {
        std::cout << "Derived constructor" << std::endl;
    }

    ~Derived() {
        std::cout << "Derived destructor" << std::endl;
    }
};

int main() {
    {
        std::cout << "aremn" << std::endl;
        aremn::shared_ptr<Base> ptr = aremn::shared_ptr<Base>::make_shared<Derived>();
    }
    {
        std::cout << "STD" << std::endl;
        std::shared_ptr<Base> ptr1 = std::make_shared<Derived>();
    }
    return 0;
}

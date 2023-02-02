class Class {
public:
    Class();
    Class(Class const&) = delete;
    Class&
    operator=(Class const&) = delete;
    Class(Class&&) = delete;
    Class&
    operator=(Class&&) = delete;
};

class Class2 {
public:
    Class2();
    static const Class* var;
};
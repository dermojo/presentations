/**
 * @file    pets.hpp
 * @brief   pet interface and implementations
 */

#ifndef SMALL_OBJECT_OPTIMIZATION_PETS_HPP_
#define SMALL_OBJECT_OPTIMIZATION_PETS_HPP_

#include <string>


/// Generic 'pet' interface.
class IPet
{
public:
    virtual ~IPet() = default;

    /// all pets can make some noise...
    virtual std::string makeSomeNoise() = 0;
};

class Dog : public IPet
{
public:
    Dog() = default;
    Dog(std::string name) : m_name(std::move(name)) {}
    virtual ~Dog() = default;

    /// Dogs can bark
    std::string makeSomeNoise() final { return "Woof, woof!"; }

private:
    /// optional dog name
    std::string m_name;
};

class Cat : public IPet
{
public:
    virtual ~Cat() = default;

    /// Cats can meow
    std::string makeSomeNoise() final { return "Meow!"; }
};

class Parrot : public IPet
{
public:
    Parrot(const char* name) { strncpy(m_name, name, sizeof(m_name)); }
    virtual ~Parrot() = default;

    /// A parrot can say its name
    std::string makeSomeNoise() final { return m_name + std::string("!"); }

private:
    char m_name[1024];
};

class Elephant : public IPet
{
public:
    // some arbitrary constructor arguments, ...
    Elephant(int height, double weight, std::string favoriteFood = "peanuts")
    {
        std::ignore = height;
        std::ignore = weight;
        std::ignore = favoriteFood;
    }

    // Elephants cannot be moved, they are too heavy
    Elephant(Elephant&&) = delete;
    Elephant& operator=(Elephant&&) = delete;

    virtual ~Elephant() = default;

    std::string makeSomeNoise() final { return "Toooooooooot!"; }
};

#endif /* SMALL_OBJECT_OPTIMIZATION_PETS_HPP_ */

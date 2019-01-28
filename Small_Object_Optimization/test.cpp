/**
 * This file contains the tests that need to pass in order to satisfy all requirements
 * to SmallPtr!
 */

#include <gtest/gtest.h>

#include "pets.hpp"

//#include "v1_unique_ptr.hpp"
#include "v2_with_stubs.hpp"
//#include "small.hpp"


// test various constructors
TEST(SmallPtr, Construct)
{
    SmallPtr<int> p1;
    ASSERT_FALSE(p1);
    ASSERT_EQ(p1.get(), nullptr);

    // default constructor
    SmallPtr<IPet> p2;
    ASSERT_FALSE(p2);
    ASSERT_EQ(p2.get(), nullptr);

    // construct a dog (no constructor args)
    SmallPtr<IPet> p3(InPlace<Dog>{});
    ASSERT_TRUE(p3);
    ASSERT_NE(p3.get(), nullptr);
    ASSERT_EQ(p3->makeSomeNoise(), "Woof, woof!");
    ASSERT_FALSE(p3.usesHeap());
    ASSERT_TRUE(p3.usesStack());

    // construct a dog (one argument)
    SmallPtr<IPet> p4(InPlace<Parrot>{}, "Lori");
    ASSERT_TRUE(p4);
    ASSERT_NE(p4.get(), nullptr);
    ASSERT_EQ(p4->makeSomeNoise(), "Lori!");
    ASSERT_TRUE(p4.usesHeap());
    ASSERT_FALSE(p4.usesStack());

    // construct an elephant (2-3 args)
    SmallPtr<IPet> p5(InPlace<Elephant>{}, 123, 123.45);
    ASSERT_TRUE(p5);
    ASSERT_NE(p5.get(), nullptr);
    ASSERT_EQ(p5->makeSomeNoise(), "Toooooooooot!");
    ASSERT_TRUE(p5.usesHeap());
    ASSERT_FALSE(p5.usesStack());
    SmallPtr<IPet> p6(InPlace<Elephant>{}, 345, 6666.6, "bananas");
    ASSERT_TRUE(p6);
    ASSERT_NE(p6.get(), nullptr);
    ASSERT_EQ(p6->makeSomeNoise(), "Toooooooooot!");
    ASSERT_TRUE(p6.usesHeap());
    ASSERT_FALSE(p6.usesStack());
}

// test .emplace()
TEST(SmallPtr, Emplace)
{
    SmallPtr<IPet> pet;

    pet.emplace<Dog>();
    ASSERT_TRUE(pet);
    ASSERT_NE(pet, nullptr);
    ASSERT_EQ(pet->makeSomeNoise(), "Woof, woof!");
    ASSERT_FALSE(pet.usesHeap());
    ASSERT_TRUE(pet.usesStack());

    pet.emplace<Parrot>("Heinrich");
    ASSERT_TRUE(pet);
    ASSERT_NE(pet.get(), nullptr);
    ASSERT_EQ(pet->makeSomeNoise(), "Heinrich!");
    ASSERT_TRUE(pet.usesHeap());
    ASSERT_FALSE(pet.usesStack());
}

// test .reset()
TEST(SmallPtr, Reset)
{
    SmallPtr<IPet> pet;

    pet.emplace<Dog>();
    ASSERT_EQ(pet->makeSomeNoise(), "Woof, woof!");

    // kill the pet :-(
    pet.reset();
    ASSERT_FALSE(pet);
    ASSERT_EQ(pet.get(), nullptr);

    // "adopt" an existing one
    pet.reset(new Parrot("Valentine"));
    ASSERT_TRUE(pet);
    ASSERT_NE(pet.get(), nullptr);
    ASSERT_EQ(pet->makeSomeNoise(), "Valentine!");
}

// make sure the optimization works...
TEST(SmallPtr, Size)
{
    // parrots don't fit by default...
    SmallPtr<IPet> smallPet(InPlace<Parrot>{}, "Bjarne");
    ASSERT_TRUE(smallPet);
    ASSERT_NE(smallPet.get(), nullptr);
    ASSERT_EQ(smallPet->makeSomeNoise(), "Bjarne!");
    // -> heap
    ASSERT_TRUE(smallPet.usesHeap());
    ASSERT_FALSE(smallPet.usesStack());

    // so increase the buffer size
    constexpr size_t parrotSize = sizeof(Parrot);
    SmallPtr<IPet, parrotSize> largePet(InPlace<Parrot>{}, "Alexandra");
    ASSERT_TRUE(largePet);
    ASSERT_NE(largePet.get(), nullptr);
    ASSERT_EQ(largePet->makeSomeNoise(), "Alexandra!");
    // -> stack
    ASSERT_FALSE(largePet.usesHeap());
    ASSERT_TRUE(largePet.usesStack());
}

TEST(SmallPtr, Move)
{
    {
        // small object
        SmallPtr<IPet> pet(InPlace<Cat>{});
        ASSERT_TRUE(pet.usesStack());
        // save the pointer for comparison
        const IPet* petPtr1 = pet.get();

        // move the cat
        SmallPtr<IPet> thief(std::move(pet));
        ASSERT_TRUE(thief);
        ASSERT_EQ(thief->makeSomeNoise(), "Meow!");
        const IPet* petPtr2 = thief.get();

        // ... the cat is gone
        ASSERT_FALSE(pet);

        // moving small objects re-locates them
        ASSERT_NE(petPtr1, petPtr2);

        // but we can construct a new pet
        pet.emplace<Dog>();
        ASSERT_EQ(pet->makeSomeNoise(), "Woof, woof!");
    }

    {
        // large object
        SmallPtr<IPet> pet(InPlace<Parrot>{}, "Dominique");
        ASSERT_TRUE(pet.usesHeap());
        // save the pointer for comparison
        const IPet* petPtr1 = pet.get();

        // move the parrot
        SmallPtr<IPet> thief(std::move(pet));
        ASSERT_TRUE(thief);
        ASSERT_EQ(thief->makeSomeNoise(), "Dominique!");
        const IPet* petPtr2 = thief.get();

        // ... the parrot is gone
        ASSERT_FALSE(pet);

        // moving heap objects just passes the pointer
        ASSERT_EQ(petPtr1, petPtr2);

        // but we can construct a new pet
        pet.emplace<Dog>();
        ASSERT_EQ(pet->makeSomeNoise(), "Woof, woof!");
    }

    {
        // a non-movable pet -> must go on the heap, even if it's "small"
        SmallPtr<IPet> pet(InPlace<Elephant>{}, 200, 1003.45);
        ASSERT_TRUE(sizeof(Elephant) <= 64);
        ASSERT_TRUE(pet.usesHeap());
        // save the pointer for comparison
        const IPet* petPtr1 = pet.get();

        // move the elephant
        SmallPtr<IPet> thief(std::move(pet));
        ASSERT_TRUE(thief);
        ASSERT_EQ(thief->makeSomeNoise(), "Toooooooooot!");
        const IPet* petPtr2 = thief.get();

        // ... the elephant is gone
        ASSERT_FALSE(pet);

        // moving heap objects just passes the pointer
        ASSERT_EQ(petPtr1, petPtr2);

        // but we can construct a new pet
        pet.emplace<Parrot>("George");
        ASSERT_EQ(pet->makeSomeNoise(), "George!");
    }
}

TEST(SmallPtr, Swap)
{
    SmallPtr<IPet> cat(InPlace<Cat>{});
    ASSERT_TRUE(cat.usesStack());
    SmallPtr<IPet> dog(InPlace<Dog>{}, "Bob");
    ASSERT_TRUE(dog.usesStack());
    SmallPtr<IPet> parrot(InPlace<Parrot>{}, "Charly");
    ASSERT_TRUE(parrot.usesHeap());

    swap(cat, dog);
    ASSERT_EQ(dog->makeSomeNoise(), "Meow!");
    ASSERT_EQ(cat->makeSomeNoise(), "Woof, woof!");

    swap(cat, parrot);
    ASSERT_EQ(parrot->makeSomeNoise(), "Woof, woof!");
    ASSERT_EQ(cat->makeSomeNoise(), "Charly!");

    dog.reset();
    swap(cat, dog);
    ASSERT_EQ(cat.get(), nullptr);
    ASSERT_EQ(dog->makeSomeNoise(), "Charly!");
}

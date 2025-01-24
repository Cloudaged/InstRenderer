
#ifndef INSTRENDERER_FLAGS_H
#define INSTRENDERER_FLAGS_H
#include "vector"
#include "bitset"
template<typename MyEnum>
class Flags
{
public:
    Flags(std::vector<MyEnum> enums);
    bool Check(MyEnum e);
    std::bitset<16> bits;
};

template<typename MyEnum>
bool Flags<MyEnum>::Check(MyEnum e)
{
    return bits.test(e);
}

template<typename MyEnum>
Flags<MyEnum>::Flags(std::vector<MyEnum> enums)
{
    for (auto& e:enums)
    {
        bits.set(e);
    }
}


#endif //INSTRENDERER_FLAGS_H

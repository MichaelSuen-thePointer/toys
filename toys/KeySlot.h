#pragma once

#ifndef KEYSLOT_H
#define KEYSLOT_H

#include "Base.h"
#include <map>
#include <vector>

namespace pl
{

template<class TKey, class TValue>
class KeySlot: public Object
{
protected:
    std::map<TKey, std::vector<TValue>> container;
public:
    KeySlot()
        : container()
    {
    }


};

}

#endif // !KEYSLOT_H

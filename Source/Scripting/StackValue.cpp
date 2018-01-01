#include "Precompiled.hpp"
#include "StackValue.hpp"
using namespace Scripting;

StackValue::StackValue(int index) :
    m_index(index)
{
}

int StackValue::GetIndex() const
{
    return m_index;
}

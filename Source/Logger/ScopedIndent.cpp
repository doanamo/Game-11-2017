#include "Precompiled.hpp"
#include "ScopedIndent.hpp"
#include "Sink.hpp"
using namespace Logger;

ScopedIndent::ScopedIndent(Sink* sink) :
    m_sink(sink)
{
    if(m_sink != nullptr)
    {
        m_sink->IncreaseIndent();
    }
}

ScopedIndent::~ScopedIndent()
{
    if(m_sink != nullptr)
    {
        m_sink->DecreaseIndent();
    }
}

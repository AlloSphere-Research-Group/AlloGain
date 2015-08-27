#pragma once

#ifdef JUCE_MAC

template<class ObjectType> class ScopedIOObject
{
public:
    ScopedIOObject() :
    object(0)
    {

    }
    
    ~ScopedIOObject()
    {
        if (object)
        {
            IOObjectRelease(object);
            object = 0;
        }
    }

    ObjectType object;
};



#endif
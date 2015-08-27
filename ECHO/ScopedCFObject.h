#pragma once

#ifdef JUCE_MAC

template<class ObjectType> class ScopedCFObject
{
public:
    ScopedCFObject() :
    object(0)
    {

    }
    
    ~ScopedCFObject()
    {
        if (object)
        {
            CFRelease(object);
            object = 0;
        }
    }

    ObjectType object;
};



#endif
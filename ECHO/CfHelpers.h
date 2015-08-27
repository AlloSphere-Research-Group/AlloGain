#ifndef _cfhelpers_h_
#define _cfhelpers_h_

class CfNumber
{
public :
	CfNumber()
	{
		_ref = NULL;
	}
	
	
	~CfNumber()
	{
		if (_ref)
			CFRelease(_ref);
	}

	CFNumberRef _ref;
};


class CfString
{
public :
	CfString()
	{
		_ref = NULL;
	}
	
	
	~CfString()
	{
		if (_ref)
			CFRelease(_ref);
	}

	CFStringRef _ref;
};


#endif

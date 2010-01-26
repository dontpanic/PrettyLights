#include <vector>
#include <map>

#define DEF_ITERATORS(c) \
	typedef c::iterator c##Iter; \
	typedef c::const_iterator c##CIter
	

// Sting vector
typedef std::vector<CString> CStringVec;
DEF_ITERATORS(CStringVec);

// 2d String Vector
typedef std::vector<CStringVec> CString2dVec;
DEF_ITERATORS(CString2dVec);

// COLORREF Vector
typedef std::vector<COLORREF> CColorRefVec;
DEF_ITERATORS(CColorRefVec);




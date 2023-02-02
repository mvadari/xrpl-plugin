#include <ripple/protocol/st.h>

template<class T>
struct DumbWrapper {
   T* wrapped_;
   T& get(){ return wrapped_;}
}
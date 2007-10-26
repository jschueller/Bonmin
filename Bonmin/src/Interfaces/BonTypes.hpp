#ifndef __BonTypes_H_
#define __BonTypes_H_
#include<vector>

namespace Bonmin {
/** A small wrap around std::vector to give easy access to array for interfacing with fortran code.*/
template<typename T>
class vector : public std::vector<T>{
public:
  /** Default constructor.*/
  vector(): std::vector<T>(){}
  /** Copy constructor.*/
  vector(const vector<T>& other): std::vector<T>(other){}
  /** Copy constructor.*/
  vector(const std::vector<T>& other): std::vector<T>(other){}
  /** constructor with size.*/
  vector(unsigned int n): std::vector<T>(n){}
  /** Assignment.*/
  vector<T>& operator=(const vector<T>& other){
     std::vector<T>::operator=(other);
     return (*this);}
  /** Assignment.*/
  vector<T>& operator=(const std::vector<T>& other){
     return std::vector<T>::operator=(other);
     return (*this);}

/** Access pointer to first element of storage.*/
inline T* operator()(){return &std::vector<T>::front();}
/** Access pointer to first element of storage.*/
inline const T* operator()() const {return &std::vector<T>::front();}
};

}
#endif

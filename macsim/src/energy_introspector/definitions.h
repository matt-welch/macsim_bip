// Definitions used for both Introspector and Zesto

#ifndef EI_DEF_H
#define EI_DEF_H

#include <utility>
#include <string>
#include <map>
#include <stdint.h>

#define TEMP_DEGREE_STEPS 101 // 300~400K
#define NONE -1
#define MAX_TIME 1e10

#define EI_CONFIG_FILEOUT
#define EI_DEBUG_

using namespace std;

class counters_t
{
 public:
  counters_t() { reset(); }

  uint64_t search; // search access
  uint64_t read; // module read access
  uint64_t write; // module write access
  uint64_t read_tag; // read_tag access (cache r/w miss)
  uint64_t write_tag; // write_tag access (tag update)

  void reset() 
  { 
    search = 0; read = 0; write = 0; read_tag = 0; write_tag = 0;
  }
};


// in unit of watt
class power_t
{
 public:
  power_t() { reset(); }

  // leakage
  double leakage; // leakage power

  // dynamic
  double baseline; // baseline dynamic power
  double search; // module entry search power
  double read; // module read power (data+tag)
  double write; // module write power (data+tag)
  double read_tag; // tag array read power
  double write_tag; // tag array write power
  double total; // total power = leakage + dynamic

  void reset()
  {
    leakage = 0.0; 
    baseline = 0.0;
    search = 0.0; read = 0.0; write = 0.0;
    read_tag = 0.0; write_tag = 0.0;
    total = 0.0;
  }

  void reset_leakage()
  {
    total = total - leakage;
    leakage = 0.0;
  }

  void reset_dynamic()
  {
    baseline = 0.0;
    search = 0.0; read = 0.0; write = 0.0;
    read_tag = 0.0; write_tag = 0.0;
    total = leakage;
  }
};


// in unit of joule
class energy_t
{
 public:
  energy_t() { reset(); }
  
  // leakage
  double leakage; // leakage energy

  // dynamic
  double baseline; // baseline dynamic energy
  double search; // module entry search energy
  double read; // module read energy (data+tag) 
  double write; // module write energy (data+tag)
  double read_tag; // tag array read energy
  double write_tag; // tag array write energy
  double total; // total energy = leakage + dynamic

  void reset()
  {
    leakage = 0.0; 
    baseline = 0.0;
    search = 0.0; read = 0.0; write = 0.0;
    read_tag = 0.0; write_tag = 0.0;
    total = 0.0;
  }

  void reset_leakage()
  {
    total = total - leakage;
    leakage = 0.0;
  }

  void reset_dynamic()
  {
    baseline = 0.0;
    search = 0.0; read = 0.0; write = 0.0;
    read_tag = 0.0; write_tag = 0.0;
    total = leakage;
  }
};

template <typename T>
class grid_t
{
 public:
  grid_t() { reset(); }

  class index_t
  {
   public:
    index_t(int X, int Y, int L) : x(X),y(Y),layer(L) { }
    int x, y;
    int layer;

    bool operator < (const index_t &index) const
    {
      if(layer == index.layer)
      {
        if(x == index.x)
          return (y < index.y);
        else
          return (x < index.x);
      }
      else
        return (layer < index.layer);
    }
  };

  double cell_width, cell_length;
  map<index_t,T> grid;

  void push(int x, int y, int layer, T data)
  {
    index_t index(x,y,layer);
    grid.insert(pair<index_t,T>(index,data));
  }
  
  void update(int x, int y, int layer, T data)
  {
    index_t index(x,y,layer);
    typename map<index_t,T>::iterator it = grid.find(index);
    
    if(it != grid.end())
      it->second = data;
    else 
      push(x,y,layer,data);
  }

  T pull(int x, int y, int layer)
  {
    T data;

    index_t index(x,y,layer);
    typename map<index_t,T>::iterator it = grid.find(index);

    if(it != grid.end())
      data = it->second;

    return data;
  }

  void reset()
  {
    cell_width = 0.0;
    cell_length = 0.0;
    grid.clear();
  }
};

// in unit of m or m^2
class dimension_t
{
 public:
  dimension_t() { reset(); }

  double x_left, y_bottom; // x-y coordinate on 2D plane
  double width, length; // width/length of rectangular block
  int layer; // 3D layer index
  double area;

  void reset()
  {
    x_left = 0.0; y_bottom = 0.0; 
    width = 0.0; length = 0.0; 
    layer = 0;
    area = 0.0;
  }

  double get_x_center()
  {
    return x_left+width/2;
  }

  double get_y_center()
  {
    return y_bottom+length/2;
  }
};


// access counter operators
inline counters_t operator+(const counters_t & x, const counters_t & y)
{
  counters_t z;
  z.search = x.search + y.search;
  z.read = x.read + y.read;
  z.write = x.write + y.write;
  z.read_tag = x.read_tag + y.read_tag;
  z.write_tag = x.write_tag + y.write_tag;
  return z;
}


inline counters_t operator-(const counters_t & x, const counters_t & y)
{
  counters_t z;
  z.search = x.search - y.search;
  z.read = x.read - y.read;
  z.write = x.write - y.write;
  z.read_tag = x.read_tag - y.read_tag;
  z.write_tag = x.write_tag - y.write_tag;
  return z;
}


// energy operators
inline energy_t operator+(const energy_t & x, const energy_t & y)
{
  energy_t z;
  z.leakage = x.leakage + y.leakage;
  z.baseline = x.baseline + y.baseline;
  z.search = x.search + y.search;
  z.read = x.read + y.read;
  z.write = x.write + y.write;
  z.read_tag = x.read_tag + y.read_tag;
  z.write_tag = x.write_tag + y.write_tag;
  z.total = x.total + y.total;
  return z;
}


inline energy_t operator-(const energy_t & x, const energy_t & y)
{
  energy_t z;
  z.leakage = x.leakage - y.leakage;
  z.baseline = x.baseline - y.baseline;
  z.search = x.search - y.search;
  z.read = x.read - y.read;
  z.write = x.write - y.write;
  z.read_tag = x.read_tag - y.read_tag;
  z.write_tag = x.write_tag - y.write_tag;
  z.total = x.total - y.total;
  return z;
}


// power operators
inline power_t operator+(const power_t & x, const power_t & y)
{
  power_t z;
  z.leakage = x.leakage + y.leakage;
  z.baseline = x.baseline + y.baseline;
  z.search = x.search + y.search;
  z.read = x.read + y.read;
  z.write = x.write + y.write;
  z.read_tag = x.read_tag + y.read_tag;
  z.write_tag = x.write_tag + y.write_tag;
  z.total = x.total + y.total;
  return z;
}


inline power_t operator-(const power_t & x, const power_t & y)
{
  power_t z;
  z.leakage = x.leakage - y.leakage;
  z.baseline = x.baseline - y.baseline;
  z.search = x.search - y.search;
  z.read = x.read - y.read;
  z.write = x.write - y.write;
  z.read_tag = x.read_tag - y.read_tag;
  z.write_tag = x.write_tag - y.write_tag;
  z.total = x.total - y.total;
  return z;
}


// energy to power conversion: power = energy*frequency
inline power_t operator*(const energy_t & x, const double y)
{
  power_t z;
  z.leakage = x.leakage * y;
  z.baseline = x.baseline * y;
  z.search = x.search * y;
  z.read = x.read * y;
  z.write = x.write * y;
  z.write_tag = x.write_tag * y;
  z.read_tag = x.read_tag * y;
  z.total = x.total * y;
  return z;
}

inline power_t operator*(const power_t & x, const double y)	
{
  power_t z;
  z.leakage = x.leakage * y;
  z.baseline = x.baseline * y;
  z.search = x.search * y;
  z.read = x.read * y;
  z.write = x.write * y;
  z.write_tag = x.write_tag * y;
  z.read_tag = x.read_tag * y;
  z.total = x.total * y;
  return z;
}

// power to energy conversion: energy = power/frequency 
inline energy_t operator/(const power_t & x, const double y)
{
  energy_t z;
  z.leakage = x.leakage / y;
  z.baseline = x.baseline / y;
  z.search = x.search / y;
  z.read = x.read / y;
  z.read_tag = x.read_tag / y;
  z.write = x.write / y;
  z.write_tag = x.write_tag / y;
  z.total = x.total / y;
  return z;
}

#endif


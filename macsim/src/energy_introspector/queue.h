#ifndef EI_STAT_H
#define EI_STAT_H

#include <map>
#include <typeinfo>
#include <iostream>
#include <math.h>

#include "energy_introspector.h"

#define max_trunc 1e-15 // maximum truncation error

class queue_t;

class queue_wrapper_t
{
 public:
  queue_wrapper_t(queue_t *q) : queue(q) {}

  queue_t *queue;
  vector<class pseudo_sensor_t*> sensor;
};

template <typename T> class runtime_queue_t;

class queue_t
{
 public:
  virtual ~queue_t() {}

 private:
  map<string,queue_wrapper_t> queue_wrapper;

 public:
  void reset(void)
  {
    queue_wrapper.clear();
  }

  bool is_queue(string name)
  {
    map<string,queue_wrapper_t>::iterator it = queue_wrapper.find(name);

    if(it == queue_wrapper.end())
      return false;
    else
      return true;
  }

  // regular data queue
  template <typename T>
  double begin(string name)
  {
    double time_tick = MAX_TIME;
    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in begin()\n",name.c_str());
        return time_tick;
      }
      time_tick = rtq->head;//rtq->window.begin()->first-rtq->window.begin()->second.period;
    }
    else
    {
      fprintf(stdout,"EI WARNING (Queue): cannot find runtime queue %s in begin()\n",name.c_str());
    }
    return time_tick;
  }

  template <typename T>
  double end(string name)
  {
    double time_tick = 0.0;
    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in end()\n",name.c_str());
        return time_tick;
      }
      time_tick = rtq->tail;//rtq->window.rbegin()->first;
    }
    else
    {
      fprintf(stdout,"EI WARNING (Queue): cannot find runtime queue %s in end()\n",name.c_str());
    }
    return time_tick;
  }

  template <typename T>
  bool is_synchronous(double time_tick, double period, string name)
  {
    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in is_synchronous()\n",name.c_str());
        return false;
      }
      return rtq->is_synchronous(time_tick,period);
    }
    else
    {
      fprintf(stdout,"EI WARNING (Queue): cannot find runtime queue %s in is_synchronous\n",name.c_str());
    }
    return false;
  }

  template <typename T>
  void create(string name, int size)
  {
    if(queue_wrapper.find(name) != queue_wrapper.end())
    {
      fprintf(stdout,"EI WARNING (Queue): skip creating runtime queue %s (duplicated)\n",name.c_str());
      return;
    }

    runtime_queue_t<T> *rtq = new runtime_queue_t<T>(size); // T is runtime_queue
    queue_wrapper.insert(pair<string,queue_wrapper_t>(name,queue_wrapper_t(rtq)));
  }

  template <typename T>
  void push(double time_tick, double period, string name, T data)
  {
    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in push()\n",name.c_str());
        return;
      }
      rtq->push(time_tick,period,data);

      if((time_tick-rtq->head > max_trunc)&&(rtq->tail-time_tick > max_trunc))
      {
        fprintf(stdout,"EI WARNING (Queue): pushing data %s overlaps the queue range\n",name.c_str());
      }
      else if(period&&!is_synchronous<T>(time_tick,period,name))
      {
        fprintf(stdout,"EI WARNING (Queue): asynchronous data push %s flushes the queue\n",name.c_str());
      }
    }
    else
    {
      #ifdef EI_DEBUG
      fprintf(stdout,"WARNING (EI): cannot find runtime queue %s in push()\n",name.c_str());
      #endif
    }
  }

  template <typename T>
  T pull(double time_tick, string name)
  {
    T data;

    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in pull()\n",name.c_str());
        return data;
      }
      data = rtq->pull(time_tick);

      if((time_tick-rtq->tail > max_trunc)||(rtq->head-time_tick > max_trunc))
      {
        //fprintf(stdout,"EI WARNING (Queue): pulling data %s out of the queue range %lf / [%lf:%lf]\n",name.c_str(),time_tick,rtq->head,rtq->tail );	// Fixme Jieun Apr-4-2012
      }
    }
    else
    {
      fprintf(stdout,"EI WARNING (Queue): cannot find runtime queue %s in pull()\n",name.c_str());
    }

    return data;
  }

  template <typename T>
  void update(double time_tick, double period, string name, T data)
  {
    if(is_queue(name))
    {
      runtime_queue_t<T> *rtq = dynamic_cast<runtime_queue_t<T>* >(queue_wrapper.find(name)->second.queue);
      if(!rtq)
      {
        fprintf(stdout,"EI WARNING (Queue): dynamic_cast fails in accessing the runtime queue %s in update()\n",name.c_str());
        return;
      }
      if(rtq->is_synchronous(time_tick,period))
        rtq->update(time_tick,period,data);
      else
      {
        fprintf(stdout,"EI WARNING (Queue): skipping asynchronous update of data %s in update()\n",name.c_str());
      }
    }
  }

  // sensor data queue
  int add_sensor(string name, pseudo_sensor_t &sensor)
  {
    if(is_queue(name))
    {
      int index = 0;
      map<string,queue_wrapper_t>::iterator it = queue_wrapper.find(name);
      vector<pseudo_sensor_t*>::iterator s_it;
      for(s_it = it->second.sensor.begin(); s_it < it->second.sensor.end(); s_it++)
      {
        if(*s_it == &sensor)
          break;
        index++;
      }
      if(s_it == it->second.sensor.end())
        queue_wrapper.find(name)->second.sensor.push_back(&sensor);
      else
      {
        fprintf(stdout,"EI WARNING (Queue): adding duplicate pseudo sensors to runtime queue %s\n",name.c_str());
      }
      return index;
    }
    else
    {
      fprintf(stdout,"EI ERROR (Queue): cannot find runtime queue %s to add a pseudo sensor\n",name.c_str());
      exit(1);
    }
  }

  pseudo_sensor_t* get_sensor(string name,int index)
  {
    if(is_queue(name))
    {
      map<string,queue_wrapper_t>::iterator it = queue_wrapper.find(name);
      if(it->second.sensor.size() > 0)
      {
        if((index >= 0)&&(index < (int)it->second.sensor.size()))
          return it->second.sensor[index];
      }
      return NULL;
    }
    else
    {
      fprintf(stdout,"EI WARNING (Queue): cannot find runtime queue %s in get_sensor()\n",name.c_str());
      return NULL;
    }
  }
};

template<typename T>
class runtime_queue_t : public queue_t
{
 private:
  class data_t{
   public:
    double period;
    T value;
  };

 public:
  runtime_queue_t<T>(int size) : window_size(size), head(0.0), tail(0.0) {}

  int window_size;
  double head, tail;
  map<double,data_t> window;

  void push(double time_tick, double period, T stat)
  {
    data_t data;
    data.period = period;
    data.value = stat;

    // backup the window to new stat's time_tick
    for(typename map<double,data_t>::reverse_iterator it = window.rbegin();
        it != window.rend(); )
    {
      if((time_tick-it->first) > max_trunc)
        break;
      else
      {
        typename map<double,data_t>::iterator dead_entry = window.find(it->first);
        it++;
        window.erase(dead_entry);
        
      }
    }

    // overlap or gap between data points - flush window
    if((window.size() > 0)&&period&&(fabs(time_tick-period-window.rbegin()->first) > max_trunc))
      window.clear();

    // insert new data
    window.insert(pair<double,data_t>(time_tick,data));

    // resize window
    while(window.size() > window_size)
      window.erase(window.begin());

    head = window.begin()->first-window.begin()->second.period;//-max_trunc;
    tail = window.rbegin()->first;//+max_trunc;
  }

  T pull(double time_tick)
  {
    T data;

    // search the map backward
    for(typename map<double,data_t>::reverse_iterator it = window.rbegin();
        it != window.rend(); it++)
    {
      if((time_tick-it->first) > max_trunc) // no need to search further
        break;
      else if(it->first-it->second.period == 0.0)
      {
        data = it->second.value; 
        break;
      }
      else if(it->second.period == 0.0) // best-effort return for unspecified period
      {
        ++it;
        if((it == window.rend())||((time_tick-it->first) > max_trunc))
        {
          --it;
          data = it->second.value;
          break;
        }        
      }
      else if((time_tick-it->first+it->second.period) > max_trunc) // found the time_tick within the interval
      {
        data = it->second.value;
        break;
      }
    }
    return data;
  }

  void update(double time_tick, double period, T stat)
  {
    if(is_synchronous(time_tick,period))
    {
      if((window.size() == 0)||((time_tick-tail) > max_trunc))
        push(time_tick,period,stat);
      else
      {
        for(typename map<double,data_t>::reverse_iterator it = window.rbegin();\
            it != window.rend(); )
        {
          if(time_tick == it->first)
          {
            it->second.value = stat;
            break;
          }
        }
      }
    }
    
    head = window.begin()->first-window.begin()->second.period;//-max_trunc;
    tail = window.rbegin()->first;//+max_trunc;
  }

  bool is_synchronous(double time_tick, double period)
  {
    if(window.size() > 0)
    {
      for(typename map<double,data_t>::reverse_iterator it = window.rbegin();
          it != window.rend(); it++)
      {
        if((time_tick-it->first) > max_trunc)
          return fabs((time_tick-period-it->first) <= max_trunc);
        else if((time_tick-it->first+it->second.period) > max_trunc)
          return ((fabs(time_tick-period-it->first+it->second.period) <= max_trunc)&&(fabs(time_tick-it->first) <= max_trunc));
      }
    }
    else 
      return (time_tick-period-tail-max_trunc <= max_trunc)||(time_tick-period-tail >= 0.0);

    return false;
  }

  void clear()
  {
    window.clear();
    head = tail;
  }
};

#endif

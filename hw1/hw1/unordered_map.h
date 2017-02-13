#ifndef _UNORDERMAP_H_
#define _UNORDERMAP_H_

#include <unordered_map>
#include <list>
#include <cstdlib>
#include <iostream>

using namespace std;

class Cache{
 private:
  int cap;
  int size;
  int minFreq;
  /* base map holds a map "absoluteURI -> <response content, frequency>" */
  unordered_map<char*, pair<char*, int>> b_map;
  unordered_map<char*, list<char*>::iterator> mIter;
  /* frequency map is used to sort the response by the frequency and thus implement the replacement policy */
  unordered_map<int, list<char*>> f_map;

 public:
  Cache(char *cap){
    this.cap = cap;
    size = 0;
  }

  char *getValue(char *key){
    if(b_map.count(key) == 0){
      return NULL;
    }
    /* erase one key from original freq list, then add it to new freq(freq+1) list */
    f_map[b_map[key].second].erase(mIter[key]);
    b_map[key].second++;
    f_map[b_map[key].second].push_back(key);
    /* get an iterator that pointing the last element in the list */
    mIter[key] = --f_map[b_map[key].second].end();

    if(f_map[minFreq].size() == 0){
      minFreq++;
    }

    return b_map[key].first;
  }

  void setValue(char *key, char *value){
    if(cap<=0){
      cerr << "Invalid capacity, unable to cache item" << endl;
      exit(1);
    }

    char* storedValue = getValue(key);
    if(storedValue != NULL){
      b_map[key].first == value;
      return;
    }

    if(size >= cap){
      /* evict the first element in minFreq list */
      /* if there is more than one element in minFreq list, use the FIFO policy to evict element */
      b_map.erase(f_map[minFrq].front());
      mIter.erase(f_map[minFrq].front());
      f_map[minFreq].pop_front();
      size--;
    }
    /* update base map and frequency map */
    b_map[key].first = value;
    b_map[key].second = 1;
    f_map[1].push_back(key);
    mIter[key] = --f_map[1].end();
    minFreq = 1;
    /* as one key,value pair added into map, increase the size */
    size++;
    
    return;
  }
};
    


#endif

////////////////////////////////////////////////////////////
//
// File: dlist.cpp
// Author: Patrick Groeneveld  
// Purpose:
//      Bodies of the classes 'dlist' and 'item'
//      These classes implement a double linked list base class
//
// Sven: Changes compared to old final task skeleton:
//      -Added an extra constructor(4) for the item class
//      -Added reset functionality for the iterator class
//
//	Hexagon, Jeroen van Oorschot 2014
//
///////////////////////////////////////////////////////////
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dlist.h"  // header file with class definition

    /////////////////////////////////////////////////////
   //
  //  dlist class
 //
////////////////////////////////////////////////////

//
// Constructor: makes empty list
//
dlist::dlist(void)
{
   _beginning_of_list = 0; // list is empty
}

//
// dtor: unchains all items (but doesn't kill them)
//
dlist::~dlist(void)
{
    //remove_all_items();
    destroy_all_items();
}

//
// Kill all items in the list
//
void dlist::destroy_all_items(void)
{
   assert(this); // if you crash here you're calling this on a 0 object
   ITER iterator(this);
   item * it;
   while((it = iterator.next()) != 0) {
      delete it;  // This will get the item out of the list
   }
   assert(_beginning_of_list == 0);  // list should be empty at the end!
}

//
// Make the list empty but do not delete the elements
// This will unchain all items of the list
//
void dlist::remove_all_items(void)
{
   assert(this); // if you crash here you're calling this on a 0 object
   ITER iterator(this);
   item * it;
   while((it = iterator.next()) != 0) {
      it->remove_from_list();
   }
   assert(_beginning_of_list == 0);
}

//
// Find an item that has the given name
// It will return a pointer to the item to the item
// It will return 0 if no item with 'name' exists
//
item * dlist::find(const char * const name) const
{
   assert(this); // if you crash here you're calling this on a 0 object
   ITER iterator(this);
   item * it;
   while((it = iterator.next()) != 0) {
      if(strcmp(name, it->name()) == 0) {
         return it; // found it
      }
   }
   return 0; // at end of list: not found
}

//
// Count the number of elements in the list
// Returns the count.
int dlist::count(void) const
{
   assert(this); // if you crash here you're calling this on a 0 object
   ITER iterator(this);
   int cnt = 0;
   while(iterator.next()) {
      cnt++;
   }
   return cnt;
}

void dlist::print(void) const
{
   assert(this); // if you crash here you're calling this on a 0 object
//   printf("List contains %d items:\n", count());
   ITER iterator(this);
   item * itm;
   while((itm = iterator.next()) != 0) {
      itm->print();
   }
}

void dlist::remove(item * const itm)
{
  assert(this); // if you crash here you're calling this on a 0 object
  assert(itm);  // if you crash here itm was 0
  itm->remove_from_list();
}

void dlist::append(item * const itm)
{
  assert(this); // if you crash here you're calling this on a 0 object
  assert(itm);  // if you crash here itm was 0
  itm->append_to_list(this);
}

void dlist::insert(item * const itm)
{
  assert(this); // if you crash here you're calling this on a 0 object
  assert(itm);  // if you crash here itm was 0
  itm->insert_into_list(this);
}

    /////////////////////////////////////////////////////
   //
  //  ITEM class
 //
////////////////////////////////////////////////////

//
// Contructor 1:
// Item default constructor without a name
//
item::item(void)
{
   _next = _prev = 0;
   _dlist = 0;    // not part of a list
   _name = 0;
   _ptr = 0;
}

//
// Contructor 2:
// Item constructor with a name but bot in a list
//
item::item(const char * const name)
{
   _next = _prev = 0;
   _dlist = 0;   // not part of a list
   _name = 0;
   set_name(name);
   _ptr = 0;
}

//
// Contructor 3:
// Item constructor with a name AND put it in a list
// immediately
//
item::item(const char * const name, dlist * const list)
{
   _next = _prev = 0;
   _dlist = 0;
   _name = 0;
   _ptr = 0;

   set_name(name);

   assert(list);  // if you crash here then you called this with no list pointer
   append_to_list(list);
}

// Extra contstructor added, which doesn't require a name
// Contructor 4:
// Item constructor: put it in a list immediately
//
item::item(dlist * const list)
{
   _next = _prev = 0;
   _dlist = 0;
   _name = 0;
   _ptr = 0;

   assert(list);  // if you crash here then you called this with no list pointer
   append_to_list(list);
}

//
// The destructor: removes itself from the list
//
item::~item(void)
{
   remove_from_list();
   set_name(0); // gets rid of the memory for _name (if any)
   _ptr = 0;
}


//
// This is how the item removes itself from the list
//
void item::remove_from_list(void)
{
   assert(this); // if you crash here you're calling this on a 0 object
   if(_dlist == 0)
      return; // not in any list
   if(_prev)
      _prev->_next = _next;  // chain out
   if(_next)
      _next->_prev = _prev;  // chain out
   if(_dlist->_beginning_of_list == this)
      _dlist->_beginning_of_list = _next;  // If we're at the beginning

   _dlist = 0;                             // not anymore in a list
   _next = _prev = 0;                      // just making sure
}

//
// Inserts this item at the beginning of 'list'
//
void item::insert_into_list(dlist * const list)
{
   assert(this); // if you crash here you're calling this on a 0 object
   assert(list); // list may not be 0: you don't want to insert in a 0 list

   if(_dlist)    // remove from any existing list
      this->remove_from_list();
   // mount in front of the first one
   _next = list->_beginning_of_list;
   _prev = 0;
   if(list->_beginning_of_list != 0)
       list->_beginning_of_list->_prev = this;
   list->_beginning_of_list = this;
   _dlist = list;
}

//
// Inserts this item at the beginning of 'list'
//
void item::append_to_list(dlist * const list)
{
   assert(this); // if you crash here you're calling this on a 0 object
   assert(list); // list may not be 0: you don't want to insert in a 0 list

   if(_dlist)    // remove from any existing list
      this->remove_from_list();
   // find the the last one
   item * itm;
   for(itm = list->_beginning_of_list; itm != 0 && itm->_next != 0; itm = itm->_next)
        /* nothing*/ ;
   if(itm == 0)
        list->_beginning_of_list = this;  // the list was empty
   else
        itm->_next = this;
   _next = 0;
   _prev = itm;
   _dlist = list;
}


//
// Makes the item print itself.
// You probably wanna overload this function
//
void item::print(void) const
{
   assert(this); // if you crash here you're calling this on a 0 object
   printf("Item name: %s\n", _name == 0 ? "<NULL>": _name);
}

//
// Makes the item print itself.
// You probably wanna overload this function
//
void item::draw(void) const    // You will have to overload this function
{
   assert(0);
}

//
// Sets the name of item to 'name'.
//
void item::set_name(const char * const name)
{
   assert(this); // if you crash here you're calling this on a 0 object
   if(_name)
      free(_name);  // free any existing name
   if(name) {
      _name = (char *) malloc((strlen(name) + 1) * sizeof(char));
      strcpy(_name, name);
   } else
      _name = 0;
}


//
// This is a little hack pointer that is maintained in the item
//
void
item::set_ptr(void * ptr)
{
   _ptr = ptr;
}

void *
item::get_ptr(void)
{
   return _ptr;
}


      //////////////////////////////////////////////////////////////
     //
    //  The iterator class dlist::ITER
   //
  //  This class allows us to step over the items in the list
 //
//////////////////////////////////////////////////////////////

//
// The constructor of the iterator
// The iterator has only 1 data member
//
dlist::ITER::ITER(const dlist * const list)
{
   _list = (dlist*) list;
   reset(); //reset and initialize the iterator
}

//
// Destructor of the iterator
//
dlist::ITER::~ITER(void)
{
// nothing to do here
}


//
// The next() member function
// Each time it is called, it will return
// the next item of the linked list
//
item * dlist::ITER::next(void)
{
   item * ret_item = _next_item;
   if(_next_item)
      _next_item = _next_item->_next; // step for next time
   return ret_item;
}

//
// The reset() member function
// Each time it is called, it will reset the iterator to the top of the list
//
void dlist::ITER::reset(void)
{
   if(_list == 0) {
      // no list: make sure we return nothing when next is called
      _next_item = 0;
   } else {
     //  initialize on first in list
      _next_item = _list->_beginning_of_list;
   }
}


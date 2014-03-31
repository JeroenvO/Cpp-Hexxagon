////////////////////////////////////////////////////////////
//
// File: dlist.h
// Author: Patrick Groeneveld
// Purpose:
//      Class definitions 'dlist' and 'item'
//      These classes implement a double linked list base class
//
// Sven: Changes compared to old final task skeleton:
//      -Added an extra constructor(4) for the item class
//      -Added reset functionality for the iterator class
///////////////////////////////////////////////////////////
// Look Mom: no includes!
#ifndef DLIST_H      // this avoid multiple inclusion of this file
#define DLIST_H


class item;  // forward

    //////////////////////////////////////////////////////
   //
  // dlist: 'owns' a list of items.
 //
//////////////////////////////////////////////////////
class dlist {
public:
   dlist(void);          // ctor: empty list
   ~dlist(void);         // dtor: unchains the items

	void   // kill all items in the list
		destroy_all_items(void);
	void   // unchain (not kill) all items of the list
		remove_all_items(void);

	void   // append an item to the end of the list
		append(item * it);
	void   // insert an item at the beginning of the list
		insert(item * it);
	void   // remove the item from the list
		remove(item * it);

   item * // find an item with a specific name
        find(const char * const name) const;
   int    // return the number of items in the list
        count(void) const;
   virtual void
        print(void) const;

private:
   item * // the head of the linked list
       _beginning_of_list; // only 4 bytes

public:
   class ITER; // the iterator class to step over the items
   friend item;  // item and ITER can fool around with this class
   friend ITER;
};


    ////////////////////////////////////////////
   //
  //  item class is an item of a double linked list
 //
///////////////////////////////////////////
class item {
public:
   item(void);                     // ctor1: without a name
   item(const char * const name);  // ctor2: with 'name'
   item(const char * const name,   // ctor3: with a name and a list
        dlist * const list);
   item(dlist * const list);       // ctor4: just a list

   virtual ~item(void);            // dtor calls this->remove_from_list()

   virtual void     // prints contents of this
      print(void) const;

   virtual void     // Draws the item
      draw(void) const;
      
   const char *     // returns the name
      name(void) const { return _name; }

   void *           // gets the pointer member
      get_ptr(void);
protected:          // protected allows access to classed that inherit
   void             // chain myself at the end of the list indicated
       append_to_list(dlist * const list);
   void             // chain myself in the beginning of the list indicated
       insert_into_list(dlist * const list);
   void             // remove myself from a list (if any)
       remove_from_list(void);
   dlist *          // the list I am in
      my_list(void) const { return _dlist; }
   void             // change my name
      set_name(const char * const name);
   void             // sets the generic pointer.
      set_ptr(void * ptr);

private:
   char * _name;    // my name (the payload)
   dlist * _dlist;  // the list I am in
   item * _next;    // chain ptr
   item * _prev;    // chain ptr
                    // total of 20 bytes per item (+ string malloc)
   void * _ptr;     // hack ptr to work-around issues with borland

   friend dlist::ITER;  // the iterator and dlist need to mess with it...
   friend dlist;
};


       /////////////////////////////////////////
      //
     // dlist::ITER Iterator subclass inside dlist that
    //  allows us to step over the items of the list.
   //
   // This is an example of you can use it:
   //
   //   dlist * mylist = new dlist();
   //   item * itm;
   //
   //   // whatever code to fill the list
   //   dlist::ITER myiter(mylist);
   //   while(itm = myiter.next()) {
   //       // the pointer itm steps over the items of mylist
   //       // do something with itm
   //   }
   //
  //
 //
///////////////////////////////////////////////
class dlist::ITER
{
public:
   ITER(const dlist * const list);   // 'list' is the one to iterate
   ~ITER(void);
   item*	next(void);	// return the next item in the list
        
   void reset(void);
private:
	item* _next_item;   // the item that is to be returned next time
	dlist* _list;
};

#endif

//
//  arraylist.h
//
//  Created by Ethan Laur on 11/12/13.
//  Copyright (c) 2013 Ethan Laur. All rights reserved.
//

/* DOCUMENTATION
 * Ignore linkedlist_type, it isnt important for users.
 * arraylist()
 *   creates a new arraylist with an empty linked list tree
 * arraylist(const arraylist<T> * old)
 *   makes a copy of any arraylist as long as the types match
 * operator[](int index)
 *   returns a read and write copy of the object at index (same as get)
 * operator+(T obj)
 *   adds obj to the list (same as add)
 * operator-(int index)
 *   remove object at index (same as remove)
 * T first()
 *   returns the first element in the list
 * T last()
 *   returns the last element in the list
 * T getro(int index)
 *   returns a the data member at index. This is read only
 * T * get(int index)
 *   returns a pointer to the data member at index. Read write.
 * int size()
 *   returns the number of elements in the list
 * void add(T obj)
 *   adds (copies) the object into the list.
 * void remove(int index)
 *   removes a data member (at index) from the list
 * void replace(int index, T obj)
 *   replaces object at index with obj
 * --------------Developer methods-------(#define arraylist_developer)--
 * linkedlist_type<T> * getlist()
 *   returns a pointer to the root node of the linked list
 * --------------If you can use the list by yourself, just #define linkedlist_type_developer--

 */

#ifndef __phyrrus9__arraylist__
#define __phyrrus9__arraylist__

#include <stdlib.h>

template <class T>
class linkedlist_type
{
#ifndef linkedlist_type_developer
private:
#else
public:
#endif
    T data;
    linkedlist_type *parent;
    linkedlist_type *next;
public: //constructors
    linkedlist_type()
    {
        parent = next = NULL;
    }
	linkedlist_type(linkedlist_type * p)
	{
		next = NULL;
		parent = p;
	}
	linkedlist_type(linkedlist_type * p, linkedlist_type * n)
	{
		next = n;
		parent = p;
	}
public: //setters
	void setparent(linkedlist_type * p)
	{
		parent = p;
	}
	void setchild(linkedlist_type * n)
	{
		next = n;
	}
	void setdata(T obj)
	{
		data = obj;
	}
public: //getters
	linkedlist_type * getparent()
	{
		return parent;
	}
	linkedlist_type * getchild()
	{
		return next;
	}
	T * getdata()
	{
		return &data;
	}
};

template <class T>
class arraylist
{
private:
	linkedlist_type<T> list; //just the allocation, never use me
	linkedlist_type<T> *listptr; //the ptr to the current element (always NULL)
	linkedlist_type<T> *list_start; //ptr to list[0]
public:
	arraylist()
	{
        	list_start = &list;
        	listptr = list_start;
	}
	
	arraylist(const arraylist<T> * old)
	{
		int i;
		for (i = 0; i < old->size(); i++)
		{
			add(*old->get(i));
		}
	}
	
	T * operator[](int index)
	{
		return get(index);
	}
	
	void operator+(T rhs)
	{
		add(rhs);
	}
	
	void operator-(int index)
	{
		remove(index);
	}
    
	T first()
	{
        	return list_start->data;
	}
    
	T last()
	{
        	linkedlist_type<T> *ptr = list_start;
        	while (ptr->getchild() != NULL)
		{
            		ptr = ptr->getchild();
		}
        	return ptr->data;
	}
    
	T getro(int index)
	{
        	int i;
        	linkedlist_type<T> *ptr = list_start;
        	for (i = 0; i <= index; i++)
		{
            		ptr = ptr->getchild();
		}
        	return *ptr->getdata();
	}
	
	T * get(int index)
	{
        	int i;
      		linkedlist_type<T> *ptr = list_start;
        	for (i = 0; i <= index; i++)
		{
            		ptr = ptr->getchild();
		}
        	return ptr->getdata();
	}
    
	int size()
	{
        	int i = 0;
        	linkedlist_type<T> *ptr = list_start;
        	while (ptr->getchild() != NULL)
        	{
            		i++;
            		ptr = ptr->getchild();
        	}
        	return i;
	}
    
	void add(T obj)
	{
        	linkedlist_type<T> *newobj = new linkedlist_type<T>;
		newobj->setdata(obj);
		newobj->setparent(listptr);
		listptr->setchild(newobj);
        	listptr = listptr->getchild();
	}
    
	void remove(int index)
	{
        	int i;
        	linkedlist_type<T> *ptr = list_start;
		if (index > size()) return;
        	for (i = 0; i <= index; i++)
		{
            		ptr = ptr->getchild();
		}
        	ptr->getparent()->setchild(ptr->getchild());
		if (ptr->getchild() != NULL) //cause some weird segmentation faults..
		{
			ptr->getchild()->setparent(ptr->getparent());
		}
        	delete ptr;
	}
    
	void replace(int index, T obj)
	{
        	int i;
        	linkedlist_type<T> *ptr = list_start;
		if (index > size()) return;
        	for (i = 0; i <= index; i++)
		{
            		ptr = ptr->next;
		}
        	ptr->data = obj;
	}
#ifdef arraylist_developer
public:
#else
protected:
#endif
	linkedlist_type<T> * getlist()
	{
        	return list_start;
	}
};

#endif /* defined(__phyrrus9__arraylist__) */

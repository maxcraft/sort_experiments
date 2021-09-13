#pragma once

#include <cstddef>

template< typename T > class llist
{
public:
	class Iter;
	struct llist_item
	{
		T value;
		llist_item *next;

		bool operator==( llist_item &item )
		{
			return value == item.value;
		}

		bool operator!=( llist_item &item )
		{
			return value != item.value;
		}

		bool operator>( llist_item &item )
		{
			return value > item.value;
		}

		bool operator>=( llist_item &item )
		{
			return value >= item.value;
		}

		bool operator<( llist_item &item )
		{
			return value < item.value;
		}

		bool operator<=( llist_item &item )
		{
			return value <= item.value;
		}

		T operator*()
		{
			return value;
		}

		llist_item( T val ):
		value( val ), next( NULL )
		{}
	};

	llist_item *head;
	llist_item *tail;

	llist():
	head( NULL ), tail( NULL )
	{}

	~llist()
	{
		llist_item *item = head;

		while( item != NULL )
		{
			llist_item *next = item->next;
			delete item;
			item = next;
		}
	}

	void put( T value )
	{
		llist_item *item = head;
		llist_item *curr_item = NULL;

		while( item && item->value <= value )
		{
			curr_item = item;
			item = curr_item->next;
		}

		insert( curr_item, value );
	}

	void merge( llist &list )
	{
		llist_item *curr = NULL;
		llist_item *it = head;

		for( auto item : list )
		{
			while( it && it->value <= item->value )
			{
				curr = it;
				it = it->next;
			}

			curr = insert( curr, item->value );
		}
	}

	typedef Iter iterator;

	class Iter
	{
	public:
		const llist_item *operator*()
		{
			return curr_item;
		}

		bool operator==( const Iter &iter) const
		{
			return curr_item == iter.curr_item;
		}

		bool operator!=( const Iter &iter) const
		{
			return curr_item != iter.curr_item;
		}

		Iter &operator++()
		{
			if( curr_item )
			{
				curr_item = curr_item->next;
			}
			return *this;
		}

		Iter operator++( int )
		{
			Iter tmp = * this;

			if( curr_item )
			{
				curr_item = curr_item->next;
			}

			return tmp;
		}

		Iter() = delete;

	private:
		llist_item *curr_item;

		Iter( llist_item *item ) :
		curr_item( item )
		{}

		friend class llist;

	};

	Iter begin()
	{
		return Iter( head );
	}

	Iter end()
	{
		return Iter( NULL );
	}

private:
	llist_item *insert( llist_item *item, T value )
	{
		if( item != NULL && item->value == value )
		{
			return NULL;
		}

		llist_item *ni = new llist_item( value );

		if( !item )
		{
			if( head )
			{
				ni->next = head;
			}
			else
			{
				tail = ni;
			}

			head = ni;
		}
		else
		{
			if( item->next )
			{
				ni->next = item->next;
			}
			else
			{
				tail = ni;
			}

			item->next = ni;
		}

		return ni;
	}

};

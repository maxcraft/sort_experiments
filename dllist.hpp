#pragma once

template< typename T> struct dllist
{
	typedef T item_t;

	item_t *head;
	item_t *tail;

	dllist():
	head( NULL ), tail( NULL )
	{}

	// dllist( dllist&& l ):
	// head( l.head ), tail( l.tail )
	// {}

	void append( item_t *item )
	{
		if( ! head )
		{
			tail = head = item;
		}
		else if( tail )
		{
			tail->next = item;
			tail = item;
		}

		tail->next = NULL;
	}

	void prepend( item_t *item )
	{
		if( ! head )
		{
			tail = head = item;
			tail->next = NULL;
		}
		else
		{
			item->next = head;
			head = item;
		}
	}

	void insert( item_t *at, item_t *item )
	{
		if( !at )
		{
			prepend( item );
			return;
		}

		if( tail == at )
		{
			tail = item;
		}

		item->next = at->next;
		at->next = item;
	}

	static item_t *advance( item_t *from, const size_t length )
	{
		item_t *retval = from;

		for( size_t i = 0; i < length && retval; ++i )
		{
			retval = retval->next;
		}

		return retval;
	}

	void remove_tail()
	{
		item_t *item = head;

		if( !item )
		{
			return;
		}

		while( item && item->next != tail )
		{
			item = item->next;
		}

		if( item )
		{
			item->next = NULL;
		}
		else
		{
			head = tail = NULL;
		}
	}

	void remove_at( size_t at )
	{
		if( !at )
		{
			remove_head();
			return;
		}

		item_t * pos = advance( head, at - 1 );
		remove_after( pos );
	}

	void remove_after( item_t *pos )
	{
		if( !pos )
		{
			return;
		}

		item_t *item = pos->next;

		if( !item )
		{
			return;
		}

		pos->next = item->next;
		item->next = NULL;

	}

	void remove_head()
	{
		if( !head )
		{
			return;
		}

		item_t *tmp = head;
		head = head->next;
		tmp->next = NULL;

		if( !head )
		{
			tail = head;
		}
	}

	bool is_empty()
	{
		return ( head == NULL );
	}

	void clear()
	{
		head = tail = NULL;
	}
};

#pragma once

#include <cstddef>
#include <bitset>
#include "mem_storage.hpp"
#include "dllist.hpp"
#include <iostream>

template< typename T > class bulk_llist
{
public:
	class Iter;
	struct item
	{
		T value;
		item *next;

		bool operator==( item &item )
		{
			return value == item.value;
		}

		bool operator!=( item &item )
		{
			return value != item.value;
		}

		bool operator>( item &item )
		{
			return value > item.value;
		}

		bool operator>=( item &item )
		{
			return value >= item.value;
		}

		bool operator<( item &item )
		{
			return value < item.value;
		}

		bool operator<=( item &item )
		{
			return value <= item.value;
		}

		T operator*()
		{
			return value;
		}

		item():
		value( 0 ), next( NULL )
		{}

		item( T val ):
		value( val ), next( NULL )
		{}

		void set( T val )
		{
			value = val;
		}
	};

	typedef struct item item_t;
	typedef mem_storage< item_t > item_storage;
	typedef dllist< item_t > list_data;

	list_data list;

	bulk_llist():
	list(), storage()
	{}

	~bulk_llist()
	{
	}

	void put( T value )
	{
		item_t *curr_item = NULL;

		if( list.tail && list.tail->value <= value )
		{
			curr_item = list.tail;
			// std::cerr << "Adding to tail. ";
		}
		else if( !list.head || list.head->value <= value )
		{
			size_t sz = size();
			size_t mid_shft = sz / 2;
			item_t *mid = get( mid_shft ), *small = list.head;

			while( sz && mid_shft )
			{
				// std::cerr << "Mid = " << ( ( mid ) ? std::to_string( mid->value ) : "NULL" ) <<
				// " mid_shft = " << mid_shft << " sz = " << sz << std::endl;

				sz = ( sz >> 1 ) + ( sz & 1 );
				mid_shft = sz / 2;

				if( value > mid->value )
				{
					small = mid;
					mid = list.advance( mid, mid_shft );
				}
				else if( value < mid->value )
				{
					mid = list.advance( small, mid_shft );
				}
				else
				{
					break;
					// return; // ignore same items.
				}
			}

			curr_item = mid;

			// std::cerr << "Final: Mid = " << ( ( mid ) ? std::to_string( mid->value ) : "NULL" );
		}

		// std::cerr << " value = " << value << std::endl;
		insert( curr_item, value );
	}

	void append( T value )
	{
		item_t *item = insert( list.tail, value );
		item->next = NULL;
	}

	void prepend( T value )
	{
		insert( NULL, value );
	}

	// void sort()
	// {
	// 	do_sort( &( list.head ), &( list.tail ) );
	// }

	void sort()
	{
		mem_storage< sort_node > sn_stor;
		dllist< sort_node > sn_queue;

		// size_t size_c = size();

		while( !( list.is_empty() ) )
		{
			sort_node *sn = sn_stor.allocate();

			// if( !sn->listA.is_empty() && !sn->listB.is_empty() )
			// {
			// 	std::cerr << "BUG: uninitialized sort node." << std::endl;
			// 	return;
			// }

			move_sorted_chunks( sn->listA, list );
			move_sorted_chunks( sn->listB, list );

			sn_queue.append( sn );

			// if( sn_stor.size() > size_c )
			// {
			// 	std::cerr << "BUG: Too many chunks." << std::endl;
			// 	return;
			// }
		}

		// std::cerr << "Chunks has been cut: " << sn_stor.size() << std::endl;

		while( !sn_queue.is_empty() )
		{
			sort_node *snA, *snB;
			snA = sn_queue.head;
			sn_queue.remove_head();
			snB = sn_queue.head;
			sn_queue.remove_head();

			if( ! snB )
			{
				snA->merge( list );
				sn_stor.free( snA );
				break;
			}
			else
			{
				list_data listA, listB;

				snA->merge( listA );
				snB->merge( listB );
				sn_stor.free( snB );

				snA->listA = listA;
				snA->listB = listB;
				sn_queue.append( snA );
			}
		}
	}

	void move_sorted_chunks( list_data &dest, list_data &src )
	{
		// std::cerr << "Chunk start." << std::endl;
		size_t count = 0;

		while( !( src.is_empty() ) )
		{
			if( dest.tail && src.head->value < dest.tail->value )
			{
				// if( dest.head == NULL || !count )
				// {
				// 	std::cerr << "BUG: wrong list state" << std::endl;
				// }
				return;
			}

			// std::cerr << src.head->value << std::endl;
			sort_node::move_head( dest, src );
			++count;
		}

		// std::cerr << "Chunk end." << std::endl;
	}

	// void do_sort( item_t **head, item_t **tail, item_t *term = NULL )
	// {
	// 	item_t *pivot = NULL;
	//
	// 	if( head && *head && *head != term )
	// 	{
	// 		pivot = *head;
	// 		item_t *next = pivot->next;
	// 		item_t *new_head = pivot, *new_tail = pivot;
	// 		pivot->next = NULL;
	//
	// 		while( next && next != term )
	// 		{
	// 			// std::cerr << "Next: " << next->value << std::endl;
	// 			item_t *tmp = next->next;
	// 			// if( tmp )
	// 			// std::cerr << "tmp: " << tmp->value << std::endl;
	//
	// 			if( pivot->value > next->value )
	// 			{
	// 				next->next = new_head;
	// 				new_head = next;
	// 			}
	// 			else
	// 			{
	// 				new_tail->next = next;
	// 				new_tail = next;
	// 				next->next = NULL;
	// 			}
	//
	// 			next = tmp;
	// 		}
	//
	// 		new_tail->next = term;
	//
	// 		if( tail )
	// 		{
	// 			*tail = new_tail;
	// 		}
	//
	// 		*head = new_head;
	//
	// 		do_sort( head, NULL, pivot );
	// 		do_sort( &( pivot->next ), tail );
	// 	}
	// }

	void merge_old( bulk_llist &mlist )
	{
		item_t *curr = NULL;
		item_t *it = list.head;

		item_storage *s = &storage;

		while( !s->has_capacity() )
		{
			s = s->append_new();
		}


		for( auto item : mlist )
		{
			while( it && it->value < item->value )
			{
				curr = it;
				it = it->next;
			}

			curr = insert( curr, item->value, s );

			while( !s->has_capacity() )
			{
				s = s->append_new();
			}
		}
	}

	void merge( const bulk_llist &mlist )
	{
		list_data listA = list, listB = mlist.list;
		list.clear();

		while( !( listA.is_empty() && listB.is_empty() ) )
		{
			if( listA.is_empty() )
			{
				copy_head( listB );
			}
			else if( listB.is_empty() )
			{
				sort_node::move_head( list, listA );
			}
			else
			{
				if( listA.head->value < listB.head->value )
				{
					sort_node::move_head( list, listA );
				}
				else
				{
					copy_head( listB );
				}
			}
		}
	}

	// src from another list
	void copy_head( list_data &src )
	{
		if( src.head )
		{
			//append( src.head->value );
			item_t *ni = storage.allocate();
			ni->value = src.head->value;

			if( list.tail )
			{
				list.tail->next = ni;
			}

			ni->next = NULL;
			list.tail = ni;

			if( !list.head )
			{
				list.head = ni;
			}

			src.head = src.head->next;

			if( !src.head )
			{
				src.tail = NULL;
			}
		}
	}

	item_t *get( size_t index )
	{
		return list.advance( list.head, index );
	}

	int size()
	{
		return storage.size();
	}

	typedef Iter iterator;

	class Iter
	{
	public:
		const item *operator*()
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
				// next = ( curr_item ) ? curr_item->next : NULL;
			}
			return *this;
		}

		Iter operator++( int )
		{
			Iter tmp = * this;

			if( curr_item )
			{
				curr_item = curr_item->next;
				// next = ( curr_item ) ? curr_item->next : NULL;
			}

			return tmp;
		}

		Iter() = delete;

	private:
		item_t *curr_item;
		// item_t *next;

		Iter( item_t *item ) :
		curr_item( item ) //, next ( ( item ) ? item->next : NULL )
		{}

		friend class bulk_llist;
	};

	Iter begin()
	{
		return Iter( list.head );
	}

	Iter end()
	{
		return Iter( NULL );
	}

	void reserve( size_t number )
	{
		size_t count = number / 64 + ( ( number % 64 ) ? 1 : 0 );
		item_storage *s = &storage;

		for( size_t i = 0; i < count; ++i )
		{
			s = s->append_new();
		}
	}

private:
	item_storage storage;

	item_t *insert( item_t *item, T value, item_storage *s = NULL )
	{
		// if( item != NULL && item->value == value )
		// {
		// 	return NULL;
		// }

		if( !s )
		{
			s = &storage;
		}

		while( !s->has_capacity() )
		{
			s = s->append_new();
		}

		item_t *ni = s->put( [ value ]( item_t *itm ) -> void{ itm->set( value ); } );

		if( !item )
		{
			list.prepend( ni );
		}
		else
		{
			list.insert( item, ni );
		}

		return ni;
	}

	struct sort_node
	{
		list_data listA;
		list_data listB;
		sort_node *next;

		sort_node():
		listA(), listB()
		{}

		sort_node( list_data &a, list_data &b ):
		listA( a ), listB( b )
		{}

		~sort_node()
		{
			listA.head = listA.tail = NULL;
			listB.head = listB.tail = NULL;
			next = NULL;
		}

		void merge( list_data &dest )
		{
			// while( !( listA.is_empty() && listB.is_empty() ) )
			// {
			// 	if( listA.is_empty() )
			// 	{
			// 		move_head( dest, listB );
			// 	}
			// 	else if( listB.is_empty() )
			// 	{
			// 		move_head( dest, listA );
			// 	}
			// 	else
			// 	{
			// 		if( listA.head->value < listB.head->value )
			// 		{
			// 			move_head( dest, listA );
			// 		}
			// 		else
			// 		{
			// 			move_head( dest, listB );
			// 		}
			// 	}
			// }

			if( listA.is_empty() )
			{
				dest = listB;
			}
			else if( listB.is_empty() )
			{
				dest = listA;
			}
			else
			{
				dest = listA;

				item_t *la_item = dest.head, *lb_item = listB.head;

				while( lb_item )
				{
					item_t *next = lb_item->next;  //save pointer to the next item.

					while( la_item != dest.tail && la_item->value < lb_item->value )
					{
						la_item = la_item->next;
					}

					if( la_item )
					{
						lb_item->next = la_item->next;
						la_item->next = lb_item;

						if( !lb_item->next )
						{
							dest.tail = lb_item;
						}
					}

					la_item = lb_item;

					lb_item = next;
				}
			}
		}

		static void move_head( list_data &dest, list_data &src )
		{
			item_t *head = src.head;

			if( !head )
			{
				return;
			}

			src.remove_head();
			dest.append( head );
		}
	};

};

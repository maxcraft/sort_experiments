#pragma once
#include <cstddef>

template< typename T > struct mem_storage
{
	static const int mem_storage_size = 64; // Don't change
	uint64_t mask;
	T elements[ mem_storage_size ];
	mem_storage *next;

	mem_storage():
	mask( 0 ), elements(), next( NULL )
	{}

	~mem_storage()
	{
		if( next )
		{
			delete next;
		}
	}

	bool has_capacity() const
	{
		return ( ~mask );
	}

	template< typename L> T *put( L&& assigner )
	{
		T *retval = NULL;

		if( has_capacity() )
		{
			const int i = __builtin_ctzll( ~mask );
			retval = elements + i;

			// elements[ i ].set( value );
			std::forward< L >( assigner )( retval );
			mask |= ( 1ull << i );
		}

		return retval;
	}

	T *allocate()
	{
		mem_storage< T > *st = this;

		while( !( st->has_capacity() ) )
		{
			st = st->append_new();
		}

		const int i = __builtin_ctzll( ~( st->mask ) );

		T *retval = ( st->elements ) + i;

		st->mask |= ( 1ull << i );
		// retval->T();
		return retval;
	}

	int get_index( T *item )
	{
		if( item >= elements && item < ( elements + 64 ) )
		{
			return ( item - elements );
		}
		else
		{
			return -1;
		}
	}

	void free( T *item )
	{
		mem_storage< T > *curr_mem = this;
		int index = -1;

		while( curr_mem && ( index = curr_mem->get_index( item ) ) < 0 )
		{
			curr_mem = curr_mem->next;
		}

		if( index < 0 || index > 63 )
		{
			return;
		}

		mask &= ~( 1ull << index );
		item->~T();
	}

	void remove( int index )
	{
		mask &= ~( 1ull << index );
	}

	mem_storage *append_new()
	{
		if( !next )
		{
			next = new mem_storage();
		}

		return next;
	}

	size_t size()
	{
		size_t retval = 0;

		mem_storage< T > *curr_mem = this;

		while( curr_mem )
		{
			retval += __builtin_popcountll( curr_mem->mask );
			curr_mem = curr_mem->next;
		}
		return retval;
	}
};

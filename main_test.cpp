#include "llist.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <climits>
#include "bulk_llist.hpp"

template< typename T > struct rnd_gen
{
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution< T > distrib;

	rnd_gen( T min, T max ):
	rd(),
	gen( rd() ^ (
		( std::mt19937::result_type ) std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count() +
		( std::mt19937::result_type ) std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count() ) ),
	distrib( min, max )
	{}

	T operator()()
	{
		return distrib( gen );
	}
};

template< typename T, template < typename > typename L > void dump_list( L< T > &list, const std::string &listName, const std::string &separator = "\n" )
{
	std::cout << listName << std::endl;

	for( auto item : list )
	{
		std::cout << item->value << separator;
	}

	std::cout << "Size: " << list.size() << std::endl;

	// std::cout << std::flush;
}

template< typename T, template< typename> typename L > size_t count_list( L< T > &list )
{
	size_t count = 0;

	for( auto item : list )
	{
		++count;
	}

	return count;
}

template< typename T, template< typename > typename L > bool is_sorted( L< T > &list )
{
	T prev_val;
	bool init = false;
	size_t size = 0;

	for( auto item : list )
	{
		++size;
		if( !init )
		{
			prev_val = item->value;
			init = true;
			continue;
		}

		if( prev_val > item->value )
		{
			std::cerr << "prev val = " << prev_val << " curr val = " << item->value << std::endl;
			return false;
		}

		prev_val = item->value;
	}

	std::cout << "Size: " << size << std::endl;

	return true;
}

template< template< typename> typename L > void test_simple()
{
	L<int> listA, listB;

	listA.put( 10 );
	listA.put( 11 );
	listA.put( 1 );
	listA.put( 3 );
	listA.put( 7 );
	listA.put( -1 );

	listB.put( 9 );
	listB.put( 12 );
	listB.put( 4 );
	listB.put( 2 );
	listB.put( 5 );
	listB.put( -2 );

	L< int > listC;

	// std::cout  << "List A:" << std::endl;
	//
	// for( auto item : listA )
	// {
	// 	std::cout << item->value << std::endl;
	// }

	dump_list( listA, "List A:" );
	dump_list( listB, "List B:" );

	dump_list( listC, "List C:" );
	listC.merge( listA );
	dump_list( listC, "List C:" );
	listC.merge( listB );
	dump_list( listC, "List C:" );
}

template< typename T, typename L> void prepopulate_list( rnd_gen< T > &gen, L &list, const int size )
{
	auto start_time = std::chrono::high_resolution_clock::now();

	for( int i = 0; i < size; ++i )
	{
		// std::cout << i << " of " << size << " \r" << std::flush;
		list.put( gen() );
		// dump_list( list, "tmp:" );
	}

	std::cout << std::endl;
	auto end_time = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast< std::chrono::milliseconds> ( end_time - start_time ).count();
	auto usec_duration = std::chrono::duration_cast< std::chrono::microseconds> ( end_time - start_time ).count();
	std::cout << "Array of " << list.size() << " filled in " << duration << " ms avg: " << usec_duration / size << " μs" << std::endl;
}

template< typename T, typename L> void prepopulate_list_unsort( rnd_gen< T > &gen, L &list, const int size )
{
	auto start_time = std::chrono::high_resolution_clock::now();

	for( int i = 0; i < size; ++i )
	{
		// std::cout << i << " of " << size << " \r" << std::flush;
		list.append( gen() );
		// dump_list( list, "tmp:" );
	}

	std::cout << std::endl;
	auto end_time = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast< std::chrono::milliseconds> ( end_time - start_time ).count();
	auto usec_duration = std::chrono::duration_cast< std::chrono::microseconds> ( end_time - start_time ).count();
	std::cout << "Array of " << list.size() << " filled in " << duration << " ms avg: " << usec_duration / size << " μs" << std::endl;
}


template< typename T, template < typename > typename L > void test_app_sort( T min, T max, const int arr_size = 1024 * 100 )
{
	rnd_gen< T > gen( min, max );
	L< T > list;

	auto start_time = std::chrono::high_resolution_clock::now();

	prepopulate_list_unsort( gen, list, arr_size );
	// dump_list( list, "unsorted list" );
	list.sort();
	// dump_list( list, "sorted list" );
	auto end_time = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast< std::chrono::milliseconds> ( end_time - start_time ).count();
	auto usec_duration = std::chrono::duration_cast< std::chrono::microseconds> ( end_time - start_time ).count();

	// dump_list( list, "sorted list" );
	std::cout << "Array of " << list.size() << " filled in " << duration << " ms avg: " << usec_duration / arr_size << " μs" << std::endl;
	std::cout << "Is list sorted: " << std::boolalpha << is_sorted( list ) << std::endl;
}

template< typename T, template < typename > typename L > void test_benchmark( T min, T max, const int arr_size = 1024 * 100 )
{
	rnd_gen< T > gen( min, max );

	L< T > listA, listB;

	listA.reserve( arr_size << 1 );

	std::cout << "generate list A: " << std::endl;
	prepopulate_list_unsort( gen, listA, arr_size );
	listA.sort();
	std::cout << "Is list A sorted: " << std::boolalpha << is_sorted( listA ) << std::endl;


	std::cout << "generate list B: " << std::endl;
	prepopulate_list_unsort( gen, listB, arr_size );
	listB.sort();
	std::cout << "Is list B sorted: " << std::boolalpha << is_sorted( listB ) << std::endl;

	std::cout << "merging test: " << std::endl;
	auto start_time = std::chrono::high_resolution_clock::now();

	listA.merge_old( listB );

	auto end_time = std::chrono::high_resolution_clock::now();

	// dump_list( listA, "Merged list:", " " );
	std::cout << std::endl;
	std::cout << "Two arrays of " << arr_size << " merged in " << std::chrono::duration_cast< std::chrono::milliseconds> ( end_time - start_time ).count() << " ms" << std::endl;
	std::cout << "Is list A (" << listA.size() << ") sorted: " << std::boolalpha << is_sorted( listA ) << std::endl;
	std::cout << "Is list B (" << listB.size() << ")sorted: " << std::boolalpha << is_sorted( listB ) << std::endl;
	// std::cout << "Actual A size: " << count_list( listA ) << std::endl;
}

static void test_bulk_prepopulate( int size = 127 )
{
	rnd_gen< int >gen( INT_MIN, INT_MAX );

	bulk_llist< int > list;

	prepopulate_list< int, bulk_llist< int > >( gen, list, size );
	// std::cout << "Size: " << list.size() << std::endl;
	// dump_list( list, "List of 127" );
	std::cout << "Is sorted: " << std::boolalpha << is_sorted( list ) << std::endl;
}

int main()
{
	// std::cout << "Simple test:" << std::endl;
	// test_simple< llist >();
	// std::cout << "Simple test complete" << std::endl;

	std::cout << "Simple bulk test:" << std::endl;
	test_simple< bulk_llist >();
	std::cout << "Simple bulk test complete" << std::endl;

	std::cout << "Prepopulate 127 items..." << std::endl;
	test_bulk_prepopulate();
	std::cout << "Prepopulating complete" << std::endl;

	// std::cout << "Benchmark test:" << std::endl;
	// test_benchmark< int, llist >( INT_MIN, INT_MAX, 100 * 1024 );
	// std::cout << "Benchmark test complete." << std::endl;

	std::cout << "Benchmark test (bulk list):" << std::endl;
	test_benchmark< int, bulk_llist >( INT_MIN, INT_MAX, 100 * 1024 );
	std::cout << "Benchmark test complete (bulk list)." << std::endl;

	// std::cout << "Append and sort test (bulk list):" << std::endl;
	// test_app_sort< int, bulk_llist >( INT_MIN, INT_MAX, 100 );
	// std::cout << "Append and sort test complete (bulk list)." << std::endl;
	//
	// std::cout << "Append and sort test (bulk list):" << std::endl;
	// test_app_sort< int, bulk_llist >( INT_MIN, INT_MAX, 1024 * 1024 );
	// std::cout << "Append and sort test complete (bulk list)." << std::endl;

	return 0;
}

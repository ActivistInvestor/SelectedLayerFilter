#pragma once

#include "dbid.h"

class AcDbObjectIdSet
{
public:

	AcDbObjectIdSet(void)
	{
	}

	AcDbObjectIdSet( const AcDbObjectIdArray& ids )
	{
		Add( ids );
	}

	AcDbObjectIdSet( const AcDbObjectIdSet& other )
	{
		Add( other );
	}

	virtual ~AcDbObjectIdSet()
	{
	}

	inline int Add( AcDbObjectIdArray& ids )
	{
		int result = 0;
		int cnt = ids.length();
		for( int i = 0; i < cnt; i++ )
		{
			if( m_ids.insert( ids[i].asOldId() ).second )
				++result;
		}
		return result;
	}

	inline bool Add( AcDbObjectId& id )
	{
		return m_ids.insert( id.asOldId() ).second;
	}

	inline int Add( const AcDbObjectIdSet& other ) // Union
	{
		int cnt = 0;
		for( constIterator it = other.m_ids.begin(); it != other.m_ids.end(); it++ )
		{
			if( m_ids.insert( *it ).second )
				++cnt;
		}
		return cnt;
	}

	inline bool Remove( AcDbObjectId& id )
	{
		return m_ids.erase( id.asOldId() ) > 0;
	}

	const inline void operator += ( AcDbObjectId id )
	{
		Add( id );
	}

	const inline void operator -= ( AcDbObjectId id )
	{
		Remove( id );
	}

	inline int Remove( AcDbObjectIdArray& ids )
	{
		int cnt = 0;
		int len = ids.length();
		for( int i = 0; i < len; i++ )
		{
			if( m_ids.erase( ids[i].asOldId() ) > 0 )
			{
				++cnt;
			}
		}
		return cnt;
	}

	inline int Remove( const AcDbObjectIdSet& other ) // Subtraction
	{
		int cnt = 0;
		for( constIterator it = other.m_ids.begin(); it != other.m_ids.end(); it++ )
		{
			if( m_ids.erase(*it) > 0 )
				++ cnt;
		}
		return cnt;
	}

	inline bool Contains( AcDbObjectId& id ) const
	{
		return m_ids.find( id.asOldId() ) != m_ids.end();
	}

	inline int Count() const
	{
		return m_ids.size();
	}

	inline void Clear()
	{
		m_ids.clear();
	}

	inline int GetObjectIds( AcDbObjectIdArray& ids ) const
	{
		int c = m_ids.size();
		if( c > 0 )
		{
			ids.setLogicalLength( c + ids.logicalLength() );
			for( constIterator it = m_ids.begin(); it != m_ids.end(); it++ )
			{
				AcDbObjectId id;
				ids.append( id.setFromOldId(*it) );
			}
		}
		return c;
	}

	typedef stdext::hash_set<Adesk::IntDbId>::const_iterator constIterator;
	typedef stdext::hash_set<Adesk::IntDbId>::iterator Iterator;

private:

	stdext::hash_set<Adesk::IntDbId> m_ids;

};


//class AcDbObjectIdSetBase
//{
//public:
//
//	AcDbObjectIdSetBase(void)
//	{
//	}
//
//	AcDbObjectIdSetBase( const AcDbObjectIdArray& ids )
//	{
//		Add( ids );
//	}
//
//	AcDbObjectIdSetBase( const AcDbObjectIdSetBase& other )
//	{
//		Add( other );
//	}
//
//	virtual ~AcDbObjectIdSetBase()
//	{
//	}
//
//	inline int Add( AcDbObjectIdArray& ids )
//	{
//		int result = 0;
//		int cnt = ids.length();
//		for( int i = 0; i < cnt; i++ )
//		{
//			if( m_ids.insert( ids[i].asOldId() ).second )
//			{
//				++result;
//				OnAdded( ids[i] );
//			}
//		}
//		return result;
//	}
//
//	inline bool Add( AcDbObjectId& id )
//	{
//		if( m_ids.insert( id.asOldId() ).second )
//		{
//			OnAdded( id );
//			return true;
//		}
//		return false;
//	}
//
//	inline int Add( const AcDbObjectIdSetBase& other ) // Union
//	{
//		int cnt = 0;
//		for( constIterator it = other.m_ids.begin(); it != other.m_ids.end(); it++ )
//		{
//			if( m_ids.insert( *it ).second )
//			{
//				AcDbObjectId id;
//				id.setFromOldId( *it );
//				OnAdded( id );
//				++cnt;
//			}
//		}
//		return cnt;
//	}
//
//	inline bool Remove( AcDbObjectId& id )
//	{
//		if( m_ids.erase( id.asOldId() ) > 0 )
//		{
//			OnErased( id );
//			return true;
//		}
//		return false;
//	}
//
//	inline int Remove( AcDbObjectIdArray& ids )
//	{
//		int cnt = 0;
//		int len = ids.length();
//		for( int i = 0; i < len; i++ )
//		{
//			if( m_ids.erase( ids[i].asOldId() ) > 0 )
//			{
//				OnErased(ids[i]);
//				++cnt;
//			}
//		}
//		return cnt;
//	}
//
//	inline int Remove( const AcDbObjectIdSetBase& other ) // Subtraction
//	{
//		int cnt = 0;
//		for( constIterator it = other.m_ids.begin(); it != other.m_ids.end(); it++ )
//		{
//			if( m_ids.erase(*it) > 0 )
//			{
//				++ cnt;
//				AcDbObjectId id;
//				id.setFromOldId( *it );
//				OnErased( id );
//			}
//		}
//		return cnt;
//	}
//
//	inline bool Contains( AcDbObjectId& id ) const
//	{
//		return m_ids.find( id.asOldId() ) != m_ids.end();
//	}
//
//	inline int Count() const
//	{
//		return m_ids.size();
//	}
//
//	virtual inline void Clear()
//	{
//		m_ids.clear();
//	}
//
//	virtual inline int GetObjectIds( AcDbObjectIdArray& ids ) const
//	{
//		int c = m_ids.size();
//		if( c > 0 )
//		{
//			ids.setLogicalLength( c + ids.logicalLength() );
//			for( constIterator it = m_ids.begin(); it != m_ids.end(); it++ )
//			{
//				AcDbObjectId id;
//				ids.append( id.setFromOldId(*it) );
//			}
//		}
//		return c;
//	}
//
//protected:
//
//	inline bool InternalAdd( AcDbObjectId& id )  // does not trigger following notifications
//	{
//		return m_ids.insert( id.asOldId() ).second;
//	}
//
//	inline bool InternalRemove( AcDbObjectId& id ) // does not trigger following notifications
//	{
//		return m_ids.erase( id.asOldId() ) > 0;
//	}
//
//	virtual void OnAdded( AcDbObjectId& id )
//	{
//	}
//
//	virtual void OnErased( AcDbObjectId& id )
//	{
//	}
//
//private:
//
//	typedef stdext::hash_set<Adesk::IntDbId>::const_iterator constIterator;
//	typedef stdext::hash_set<Adesk::IntDbId>::iterator Iterator;
//	stdext::hash_set<Adesk::IntDbId> m_ids;
//
//};
//
///// An ordered set that stores a copy of the elements
///// in the order items were added, in an AcDbObjectIdArray
//
//class OrderedAcDbObjectIdSet : public AcDbObjectIdSetBase
//{
//public:
//
//	OrderedAcDbObjectIdSet() 
//	{
//	}
//
//	OrderedAcDbObjectIdSet( const AcDbObjectIdArray& ids ) : AcDbObjectIdSetBase( ids )
//	{
//	}
//
//	OrderedAcDbObjectIdSet( const OrderedAcDbObjectIdSet& other ) : AcDbObjectIdSetBase( other )
//	{
//	}
//
//	OrderedAcDbObjectIdSet( const AcDbObjectIdSetBase& other ) : AcDbObjectIdSetBase( other )
//	{
//	}
//
//	virtual void Clear()
//	{
//		AcDbObjectIdSetBase::Clear();
//		m_ids.setLogicalLength(0);
//	}
//
//	// Gets the ordered set
//	virtual inline int GetObjectIds( AcDbObjectIdArray& ids ) const
//	{
//		int c = m_ids.length();
//		if( c > 0 )
//			ids.append( m_ids );
//		return c;
//	}
//
//	// Derived types will not get the OnAdded() notification
//	virtual inline bool InsertAt( int index, AcDbObjectId id )
//	{
//		if( AcDbObjectIdSetBase::InternalAdd( id ) ) 
//		{
//			m_ids.insertAt( index, id );
//			return true;
//		}
//		return false;
//	}
//
//	inline void RemoveLast()
//	{
//		if( m_ids.length() > 0 )
//		{
//			AcDbObjectIdSetBase::Remove( m_ids.last() );
//		}
//	}
//
//	inline AcDbObjectId& Last() 
//	{
//		return m_ids.last();
//	}
//
//	inline const AcDbObjectId& Last() const
//	{
//		return m_ids.last();
//	}
//
//protected:
//
//	virtual void OnAdded( AcDbObjectId& id )
//	{
//		m_ids.append( id );
//	}
//
//	virtual void OnErased( AcDbObjectId& id )
//	{
//		m_ids.remove( id );
//	}
//
//private:
//
//	AcDbObjectIdArray m_ids;
//
//};
//
//class AcDbObjectIdQueue : public OrderedAcDbObjectIdSet
//{
//
//public:
//
//	AcDbObjectIdQueue( const int maxSize ) : m_size( maxSize )
//	{
//	}
//
//	/// These need to be fixed to add only maxSize elements
//
//	AcDbObjectIdQueue( const AcDbObjectIdArray& ids ) : OrderedAcDbObjectIdSet( ids )
//	{
//	}
//
//	AcDbObjectIdQueue( const AcDbObjectIdQueue& other ) : OrderedAcDbObjectIdSet( other )
//	{
//	}
//
//	AcDbObjectIdQueue( const OrderedAcDbObjectIdSet& other ) : OrderedAcDbObjectIdSet( other )
//	{
//	}
//
//	bool Insert( AcDbObjectId& id )
//	{
//		if( OrderedAcDbObjectIdSet::InsertAt( 0, id ) )
//		{
//			if( this->Count() > m_size )
//				RemoveLast();
//			return true;
//		}
//		return false;
//	}
//
//	inline int GetMaxSize()
//	{
//		return m_size;
//	}
//
//protected:
//
//	virtual void OnAdded( AcDbObjectId& id )
//	{
//		OrderedAcDbObjectIdSet::OnAdded( id );
//		int c = this->Count();
//		if( c > 1 && c > m_size )
//			RemoveLast();
//	}
//
//
//private:
//
//	int m_size;
//
//};

class LayerFilterArray
{
public:

	LayerFilterArray()
	{
	}

	~LayerFilterArray()
	{
		Clear();
	}

	void Clear()
	{
		int cnt = m_array.length();
		for( int i = 0; i < cnt; i++ )
		{
			AcLyLayerFilter* filter = m_array[i];
			if( filter != NULL )
				delete filter;
		}
		m_array.setLogicalLength(0);
	}

	void Add( AcLyLayerFilter* filter )
	{
		m_array.append( filter );
	}

	void Remove( AcLyLayerFilter* filter )
	{
		m_array.remove( filter );
	}

	void GetAll( AcArray<AcLyLayerFilter*> arr ) const
	{
		arr.append( m_array );
	}

	const AcArray<AcLyLayerFilter*>& Items()
	{
		return m_array;
	}

	AcLyLayerFilter* operator[](int index) const
	{
		return m_array[index];
	}

	int Count() const
	{
		return m_array.length();
	}

private:
	AcArray<AcLyLayerFilter*> m_array;
};

template <class TKey, class TValue>      
class CMap
{

public:

	CMap() 
	{
	}
	
	virtual ~CMap()
	{
		Clear();
	};

	// bool to indicate if insertion succeeded
	void Add( TKey key, TValue value )
	{
		m_map.insert( Pair( key, value ) );
	}

	virtual void Remove( TKey key )
	{
		m_map.erase(key);
	}

	void Clear()
	{
		m_map.clear();
	}

	bool TryGetValue( TKey key, TValue* value )
	{
		Iterator it = m_map.find(pDoc);
		if( it != m_map.end() )
		{
			*value = (*it).second;
			return true;
		}
		return false;
	}

	TValue& GetValue( TKey key ) 
	{
		Iterator it = m_map.find( key );
		if( it != m_map.end() )
			return (TValue&) ((*it).second);
		else
		{
			TValue v;
			m_map[key] = v;
			Iterator it = m_map.find( key );
			return (*it).second;
		}
	}

	inline bool Contains( TKey key ) const
	{
		return m_map.find( key ) != m_map.end();
	}

	TValue& operator[]( TKey key ) 
	{
		return GetValue( key );
	}

	int Count()
	{
		return m_map.size();
	}

	int GetKeys( AcArray<TKey> outArray )
	{
		Iterator it = m_map.begin();
		for( Iterator it = m_map.begin(); it != m_map.end(); it ++ )
		{
			outArray.append( (*it).first );
		}
		return m_map.size();
	}

	int GetValues( AcArray<TValue> outArray )
	{
		Iterator it = m_map.begin();
		for( Iterator it = m_map.begin(); it != m_map.end(); it ++ )
		{
			outArray.append( (*it).second );
		}
		return m_map.size();
	}

	typedef typename std::pair<TKey, TValue> Pair;
	typedef typename std::map<TKey, TValue>::iterator Iterator;
	typedef typename std::map<TKey, TValue>::const_iterator const_Iterator;

	Iterator Begin()
	{
		return m_map.begin();
	}

	Iterator End()
	{
		return m_map.end();
	}

private:

	std::map<TKey, TValue> m_map;

};
